(function () {
  // Message identifiers, first byte of every binary socket message.
  // Keep in sync with WebSocketMessage in src/web/TurretWebServer.h.
  const MESSAGE_RADAR = 0x01;
  const MESSAGE_MOTION = 0x02;
  const MESSAGE_ORIENTATION = 0x03;

  // Radar: sensor sits at the bottom-center of the canvas, forward is up.
  // Positions are in mm.
  const RADAR_MAX_RANGE_MM = 3000;
  const RADAR_ARC_STEP_MM = 500;
  const RADAR_TARGET_BYTES = 14;

  // Accelerometer graphs. Size and colors come from CSS / data-color on the canvases.
  const GRAPH_HISTORY = 200;          // samples kept per axis
  // Value range per graph comes from data-min / data-max on its canvas.
  // Payload order: instant x, y, z followed by smoothed x, y, z.
  const ACCEL_AXES = ['x', 'y', 'z', 'smooth-x', 'smooth-y', 'smooth-z'];
  const ORIENTATION_AXES = ['roll', 'pitch'];
  // When served from 127.0.0.1 there is no webserver, so fake the data.
  const isLocalTest = location.hostname === '127.0.0.1';

  const canvas = document.getElementById('radar');
  const ctx = canvas.getContext('2d');
  const connection = document.getElementById('connection');

  let targets = [];

  const originX = canvas.width / 2;
  const originY = canvas.height - 10;
  const pixelsPerMm = (canvas.width / 2 - 10) / RADAR_MAX_RANGE_MM;

  function drawGuides() {
    ctx.strokeStyle = '#aaa';
    ctx.fillStyle = '#666';
    ctx.lineWidth = 1;
    ctx.font = '12px sans-serif';
    for (let mm = RADAR_ARC_STEP_MM; mm <= RADAR_MAX_RANGE_MM; mm += RADAR_ARC_STEP_MM) {
      const radius = mm * pixelsPerMm;
      ctx.beginPath();
      ctx.arc(originX, originY, radius, Math.PI, 2 * Math.PI);
      ctx.stroke();
      ctx.fillText((mm / 1000) + ' m', originX + 4, originY - radius - 2);
    }
    ctx.beginPath();
    ctx.moveTo(originX - RADAR_MAX_RANGE_MM * pixelsPerMm, originY);
    ctx.lineTo(originX + RADAR_MAX_RANGE_MM * pixelsPerMm, originY);
    ctx.moveTo(originX, originY);
    ctx.lineTo(originX, originY - RADAR_MAX_RANGE_MM * pixelsPerMm);
    ctx.stroke();
  }

  function drawTargets() {
    targets.forEach(function (target) {
      if (!target.available) {
        return;
      }
      const px = originX + target.x * pixelsPerMm;
      const py = originY - target.y * pixelsPerMm;
      ctx.fillStyle = target.isMoving ? '#c62828' : '#1a7f37';
      ctx.beginPath();
      ctx.arc(px, py, 6, 0, 2 * Math.PI);
      ctx.fill();
      ctx.fillStyle = '#222';
      ctx.fillText(String(target.id), px + 9, py + 4);
    });
  }

  function draw() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    drawGuides();
    drawTargets();
  }

  function parseRadar(view) {
    const count = view.getUint8(1);
    const parsed = [];
    for (let i = 0; i < count; i++) {
      const o = 2 + i * RADAR_TARGET_BYTES;
      const flags = view.getUint8(o + 1);
      parsed.push({
        id: view.getUint8(o),
        available: (flags & 0x01) !== 0,
        isMoving: (flags & 0x02) !== 0,
        x: view.getInt16(o + 2, true),
        y: view.getInt16(o + 4, true),
        previousX: view.getInt16(o + 6, true),
        previousY: view.getInt16(o + 8, true),
        speed: view.getInt16(o + 10, true),
        resolution: view.getUint16(o + 12, true)
      });
    }
    return parsed;
  }

  function makeGraphs(axes) {
    return axes.map(function (axis, index) {
      const el = document.getElementById('graph-' + axis);
      return { axis: axis, index: index, canvas: el, ctx: el.getContext('2d'),
               color: el.dataset.color || '#fff', samples: [],
               min: parseFloat(el.dataset.min), max: parseFloat(el.dataset.max) };
    });
  }
  const accelGraphs = makeGraphs(ACCEL_AXES);
  const orientationGraphs = makeGraphs(ORIENTATION_AXES);
  const allGraphs = accelGraphs.concat(orientationGraphs);

  function drawGraph(graph) {
    const c = graph.canvas;
    const g = graph.ctx;
    // Match the backing store to the CSS size so it stays sharp when restyled.
    const dpr = window.devicePixelRatio || 1;
    const w = Math.round(c.clientWidth * dpr);
    const h = Math.round(c.clientHeight * dpr);
    if (c.width !== w || c.height !== h) {
      c.width = w;
      c.height = h;
    }
    const style = getComputedStyle(document.documentElement);
    g.clearRect(0, 0, w, h);

    const pad = 2 * dpr;
    const toY = function (value) {
      const clamped = Math.max(graph.min, Math.min(graph.max, value));
      return h - pad - (clamped - graph.min) / (graph.max - graph.min) * (h - 2 * pad);
    };
    const midY = toY((graph.min + graph.max) / 2);
    g.lineWidth = 1;
    g.strokeStyle = style.getPropertyValue('--graph-grid').trim() || '#2a313a';
    g.beginPath();
    g.moveTo(0, midY);
    g.lineTo(w, midY);
    g.stroke();

    g.fillStyle = graph.color;
    g.font = (11 * dpr) + 'px sans-serif';
    const last = graph.samples[graph.samples.length - 1];
    g.fillText((graph.canvas.dataset.label || graph.axis.toUpperCase()) + (last === undefined ? '' : ' ' + last.toFixed(2)),
               6 * dpr, 14 * dpr);

    g.strokeStyle = graph.color;
    g.lineWidth = 2 * dpr;
    g.beginPath();
    graph.samples.forEach(function (value, i) {
      const px = w - (graph.samples.length - 1 - i) * (w / (GRAPH_HISTORY - 1));
      const py = toY(value);
      if (i === 0) { g.moveTo(px, py); } else { g.lineTo(px, py); }
    });
    g.stroke();
  }

  function pushValues(graphs, values) {
    graphs.forEach(function (graph) {
      graph.samples.push(values[graph.index]);
      if (graph.samples.length > GRAPH_HISTORY) {
        graph.samples.shift();
      }
      drawGraph(graph);
    });
  }

  function parseFloats(view) {
    // After the id byte the payload is a run of little-endian f32 values.
    const values = [];
    for (let o = 1; o + 4 <= view.byteLength; o += 4) {
      values.push(view.getFloat32(o, true));
    }
    return values;
  }

  function onMessage(event) {
    const view = new DataView(event.data);
    switch (view.getUint8(0)) {
      case MESSAGE_RADAR:
        targets = parseRadar(view);
        draw();
        break;
      case MESSAGE_MOTION:
        pushValues(accelGraphs, parseFloats(view));
        break;
      case MESSAGE_ORIENTATION:
        pushValues(orientationGraphs, parseFloats(view));
        break;
    }
  }

  function connect() {
    const protocol = location.protocol === 'https:' ? 'wss://' : 'ws://';
    const socket = new WebSocket(protocol + location.host + '/ws');
    socket.binaryType = 'arraybuffer';
    socket.onopen = function () { connection.textContent = 'Connected'; };
    socket.onmessage = onMessage;
    socket.onclose = function () {
      connection.textContent = 'Disconnected, retrying…';
      setTimeout(connect, 1000);
    };
  }

  draw();
  allGraphs.forEach(drawGraph);
  let fakeSmooth = [0, 0, 9.8];
  if (isLocalTest) {
    connection.textContent = 'Test mode (fake data)';
    // Offset sine waves per axis; z rides around 1 g like a resting sensor.
    setInterval(function () {
      const t = Date.now() / 1000;
      const instant = [
        8 * Math.sin(t * 2) + 3 * Math.sin(t * 17),
        8 * Math.sin(t * 1.3 + 2) + 3 * Math.sin(t * 19),
        9.8 + 4 * Math.sin(t * 0.7 + 4) + 3 * Math.sin(t * 23)
      ];
      fakeSmooth = instant.map(function (v, i) {
        return fakeSmooth[i] + (v - fakeSmooth[i]) * 0.15;
      });
      pushValues(accelGraphs, instant.concat(fakeSmooth));
      pushValues(orientationGraphs, [
        180 * Math.sin(t * 0.5),
        90 * Math.sin(t * 0.8 + 1)
      ]);
    }, 50);
  } else {
    connect();
  }
})();
