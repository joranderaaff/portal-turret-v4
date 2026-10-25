(function () {
  const statusEl = document.getElementById('status');
  const anglesEl = document.getElementById('angles');
  const joystick = document.getElementById('joystick');
  const knob = document.getElementById('joystick-knob');
  const fireButton = document.getElementById('fire');
  // When served from 127.0.0.1 there is no webserver.
  const isLocalTest = location.hostname === '127.0.0.1';

  // Flip a sign if the turret turns the wrong way for a joystick direction.
  const SIGN_X = 1;  // joystick up
  const SIGN_Z = -1; // joystick right
  const AIM_INTERVAL_MS = 50;
  // The turret stops firing when it hears nothing for a second, see RemoteState.cpp.
  const FIRE_REPEAT_MS = 250;

  let maxX = 30;
  let maxZ = 30;
  let stick = { x: 0, y: 0 }; // -1..1, y up
  let lastSent = null;
  let aimTimer = null;
  let fireTimer = null;

  function setStatus(text, cls) {
    statusEl.textContent = text;
    statusEl.className = cls || '';
  }

  function post(path, params, okText) {
    const body = new URLSearchParams(params).toString();
    if (isLocalTest) {
      console.log('POST ' + path + ' ' + body + ' (test mode)');
      if (okText) setStatus(okText, 'ok');
      return;
    }
    fetch(path, {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: body
    })
      .then(function (res) {
        if (!res.ok) throw new Error('HTTP ' + res.status);
        if (okText) setStatus(okText, 'ok');
      })
      .catch(function (err) {
        setStatus('Failed: ' + err.message, 'error');
      });
  }

  function loadSettings() {
    if (isLocalTest) return;
    fetch('/settings')
      .then(function (res) { return res.json(); })
      .then(function (settings) {
        settings.forEach(function (setting) {
          if (setting.key === 'AngleMaxX') maxX = setting.value;
          if (setting.key === 'AngleMaxZ') maxZ = setting.value;
        });
      })
      .catch(function (err) {
        setStatus('Could not load settings, using ' + maxX + ' deg: ' + err.message, 'error');
      });
  }

  function angles() {
    return { x: SIGN_X * stick.y * maxX, z: SIGN_Z * stick.x * maxZ };
  }

  function showStick() {
    const radius = joystick.clientWidth / 2 - knob.clientWidth / 2;
    knob.style.transform = 'translate(' + stick.x * radius + 'px,' + -stick.y * radius + 'px)';
    const a = angles();
    anglesEl.textContent = 'X ' + a.x.toFixed(1) + ' Z ' + a.z.toFixed(1);
  }

  function sendAim() {
    const a = angles();
    const key = a.x.toFixed(1) + ',' + a.z.toFixed(1);
    if (key === lastSent) return;
    lastSent = key;
    post('/remote/aim', { x: a.x.toFixed(1), z: a.z.toFixed(1) });
  }

  function moveStick(event) {
    const rect = joystick.getBoundingClientRect();
    let x = (event.clientX - (rect.left + rect.width / 2)) / (rect.width / 2);
    let y = -(event.clientY - (rect.top + rect.height / 2)) / (rect.height / 2);
    const length = Math.hypot(x, y);
    if (length > 1) {
      x /= length;
      y /= length;
    }
    stick = { x: x, y: y };
    showStick();
  }

  joystick.addEventListener('pointerdown', function (event) {
    joystick.setPointerCapture(event.pointerId);
    moveStick(event);
    sendAim();
    aimTimer = setInterval(sendAim, AIM_INTERVAL_MS);
  });
  joystick.addEventListener('pointermove', function (event) {
    if (joystick.hasPointerCapture(event.pointerId)) moveStick(event);
  });

  function releaseStick() {
    if (aimTimer === null) return;
    clearInterval(aimTimer);
    aimTimer = null;
    stick = { x: 0, y: 0 };
    showStick();
    lastSent = null;
    sendAim();
  }
  joystick.addEventListener('pointerup', releaseStick);
  joystick.addEventListener('pointercancel', releaseStick);

  function sendFire(fire) {
    post('/remote/fire', { fire: fire ? 'true' : 'false' });
  }

  function startFiring(event) {
    if (fireTimer !== null) return;
    fireButton.setPointerCapture(event.pointerId);
    fireButton.classList.add('held');
    sendFire(true);
    fireTimer = setInterval(function () { sendFire(true); }, FIRE_REPEAT_MS);
  }

  function stopFiring() {
    if (fireTimer === null) return;
    clearInterval(fireTimer);
    fireTimer = null;
    fireButton.classList.remove('held');
    sendFire(false);
  }

  fireButton.addEventListener('pointerdown', startFiring);
  fireButton.addEventListener('pointerup', stopFiring);
  fireButton.addEventListener('pointercancel', stopFiring);
  fireButton.addEventListener('contextmenu', function (event) { event.preventDefault(); });
  // A page that goes to the background never sees the release.
  document.addEventListener('visibilitychange', function () {
    if (document.hidden) {
      stopFiring();
      releaseStick();
    }
  });

  document.getElementById('open-wings').addEventListener('click', function () {
    post('/remote/wings', { action: 'open' }, 'Opening wings');
  });
  document.getElementById('close-wings').addEventListener('click', function () {
    post('/remote/wings', { action: 'close' }, 'Closing wings');
  });

  document.getElementById('party-mode').addEventListener('click', function () {
    post('/remote/party', {}, 'Party mode');
  });

  loadSettings();
  showStick();
})();
