(function () {
  const statusEl = document.getElementById('status');
  // When served from 127.0.0.1 there is no webserver.
  const isLocalTest = location.hostname === '127.0.0.1';

  // Keys match the names in TurretWebServer.cpp.
  const SERVOS = [
    { key: 'rotate_x', label: 'Rotate X' },
    { key: 'rotate_z', label: 'Rotate Z' },
    { key: 'wing_left', label: 'Wing left' },
    { key: 'wing_right', label: 'Wing right' },
    { key: 'gun_left', label: 'Gun left' },
    { key: 'gun_right', label: 'Gun right' }
  ];
  const AUDIO = [
    { key: 'activate', label: 'Activate' },
    { key: 'searching', label: 'Searching' },
    { key: 'pickup', label: 'Pickup' },
    { key: 'tipped', label: 'Tipped' },
    { key: 'retire', label: 'Retire' },
    { key: 'alarm', label: 'Alarm' }
  ];

  function setStatus(text, cls) {
    statusEl.textContent = text;
    statusEl.className = cls || '';
  }

  function post(path, params, okText) {
    const body = new URLSearchParams(params).toString();
    if (isLocalTest) {
      console.log('POST ' + path + ' ' + body + ' (test mode)');
      setStatus(okText, 'ok');
      return;
    }
    fetch(path, {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: body
    })
      .then(function (res) {
        if (!res.ok) throw new Error('HTTP ' + res.status);
        setStatus(okText, 'ok');
      })
      .catch(function (err) {
        setStatus('Failed: ' + err.message, 'error');
      });
  }

  const servosEl = document.getElementById('servos');
  SERVOS.forEach(function (servo) {
    const form = document.createElement('form');
    form.className = 'field';

    const label = document.createElement('label');
    label.htmlFor = 'servo-' + servo.key;
    label.textContent = servo.label;

    const input = document.createElement('input');
    input.type = 'number';
    input.id = 'servo-' + servo.key;
    input.min = 0;
    input.max = 180;
    input.step = 1;
    input.value = 90;

    const button = document.createElement('button');
    button.type = 'submit';
    button.textContent = 'Set';

    form.append(label, input, button);
    form.addEventListener('submit', function (event) {
      event.preventDefault();
      const value = parseInt(input.value, 10);
      if (isNaN(value) || value < 0 || value > 180) {
        setStatus(servo.label + ': enter 0-180', 'error');
        return;
      }
      post('/diagnose/servo', { servo: servo.key, value: value }, servo.label + ' set to ' + value);
    });
    servosEl.appendChild(form);
  });

  document.getElementById('led-test').addEventListener('click', function () {
    post('/diagnose/leds', {}, 'LED test started');
  });

  const audioSelect = document.getElementById('audio-type');
  AUDIO.forEach(function (audio) {
    const option = document.createElement('option');
    option.value = audio.key;
    option.textContent = audio.label;
    audioSelect.appendChild(option);
  });

  document.getElementById('audio-form').addEventListener('submit', function (event) {
    event.preventDefault();
    const option = audioSelect.options[audioSelect.selectedIndex];
    post('/diagnose/audio', { audio: audioSelect.value }, 'Playing ' + option.textContent);
  });
})();
