(function () {
  // Message identifiers, first byte of every binary socket message.
  // Keep in sync with WebSocketMessage in src/web/TurretWebServer.h.
  const MESSAGE_RADAR = 0x01;

  // Radar: sensor sits at the bottom-center of the canvas, forward is up.
  // Positions are in mm.
  const RADAR_MAX_RANGE_MM = 3000;
  const RADAR_ARC_STEP_MM = 500;
  const RADAR_TARGET_BYTES = 14;

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

  function onMessage(event) {
    const view = new DataView(event.data);
    switch (view.getUint8(0)) {
      case MESSAGE_RADAR:
        targets = parseRadar(view);
        draw();
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
  connect();
})();
