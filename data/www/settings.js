(function () {
  const form = document.getElementById('settings-form');
  const fieldsEl = document.getElementById('fields');
  const statusEl = document.getElementById('status');

  // When served from 127.0.0.1 there is no webserver, so use test data instead.
  const isLocalTest = location.hostname === '127.0.0.1';

  const TEST_SETTINGS = [
    { key: 'AngleOffsetX', label: 'Angle offset X', type: 'int', value: 0, default: 0, min: -90, max: 90 },
    { key: 'AngleOffsetZ', label: 'Angle offset Z', type: 'int', value: 0, default: 0, min: -90, max: 90 },
    { key: 'Test', label: 'Test', type: 'float', value: 10.5, default: 10.5, min: -90, max: 90 },
    { key: 'TestBool', label: 'Test bool', type: 'bool', value: true, default: false },
    { key: 'TestString', label: 'Test string', type: 'string', value: 'hello', default: 'turret' }
  ];

  let settings = [];

  function setStatus(text, cls) {
    statusEl.textContent = text;
    statusEl.className = cls || '';
  }

  function clamp(value, min, max) {
    if (typeof min === 'number' && value < min) return min;
    if (typeof max === 'number' && value > max) return max;
    return value;
  }

  // Parse and clamp a numeric input; falls back to the default when empty/invalid.
  function normalizeNumber(input, setting) {
    const parse = setting.type === 'int' ? parseInt : parseFloat;
    let value = parse(input.value, 10);
    if (isNaN(value)) value = setting.default;
    value = clamp(value, setting.min, setting.max);
    input.value = value;
  }

  // Block keystrokes that can never be part of a valid number.
  function filterNumberKeys(e, isInt) {
    if (e.ctrlKey || e.metaKey || e.altKey || e.key.length > 1) return;
    const allowed = isInt ? /[0-9-]/ : /[0-9.\-]/;
    if (!allowed.test(e.key)) e.preventDefault();
  }

  function setInputValue(input, setting, value) {
    if (setting.type === 'bool') input.checked = Boolean(value);
    else input.value = value;
  }

  function createField(setting) {
    const wrapper = document.createElement('div');
    wrapper.className = 'field';

    const id = 'setting-' + setting.key;
    const label = document.createElement('label');
    label.htmlFor = id;
    label.textContent = setting.label || setting.key;

    const input = document.createElement('input');
    input.id = id;
    input.name = setting.key;

    switch (setting.type) {
      case 'int':
      case 'float':
        input.type = 'number';
        input.step = setting.type === 'int' ? '1' : 'any';
        if (typeof setting.min === 'number') input.min = setting.min;
        if (typeof setting.max === 'number') input.max = setting.max;
        input.addEventListener('keydown', function (e) {
          filterNumberKeys(e, setting.type === 'int');
        });
        input.addEventListener('change', function () {
          normalizeNumber(input, setting);
        });
        input.addEventListener('blur', function () {
          normalizeNumber(input, setting);
        });
        break;
      case 'bool':
        input.type = 'checkbox';
        break;
      case 'string':
        input.type = 'text';
        break;
      default:
        return null;
    }

    setInputValue(input, setting, setting.value);

    const reset = document.createElement('button');
    reset.type = 'button';
    reset.textContent = 'Reset';
    reset.title = 'Reset to default (' + setting.default + ')';
    reset.addEventListener('click', function () {
      setInputValue(input, setting, setting.default);
    });

    wrapper.appendChild(label);
    wrapper.appendChild(input);
    wrapper.appendChild(reset);

    if (setting.type === 'int' || setting.type === 'float') {
      const hint = document.createElement('span');
      hint.className = 'hint';
      hint.textContent = 'Range: ' + setting.min + ' to ' + setting.max +
        ' (default ' + setting.default + ')';
      wrapper.appendChild(hint);
    }

    return wrapper;
  }

  function render() {
    fieldsEl.textContent = '';
    settings.forEach(function (setting) {
      const field = createField(setting);
      if (field) fieldsEl.appendChild(field);
    });
  }

  function load() {
    if (isLocalTest) {
      settings = TEST_SETTINGS;
      render();
      return;
    }

    fetch('/settings')
      .then(function (res) {
        if (!res.ok) throw new Error('HTTP ' + res.status);
        return res.json();
      })
      .then(function (data) {
        settings = data;
        render();
      })
      .catch(function (err) {
        fieldsEl.textContent = '';
        setStatus('Failed to load settings: ' + err.message, 'error');
      });
  }

  form.addEventListener('submit', function (e) {
    e.preventDefault();

    // Build the body manually so unchecked checkboxes are sent as "false".
    const body = new URLSearchParams();
    settings.forEach(function (setting) {
      const input = form.elements[setting.key];
      if (!input) return;
      if (setting.type === 'bool') {
        body.append(setting.key, input.checked ? 'true' : 'false');
      } else {
        if (setting.type === 'int' || setting.type === 'float') {
          normalizeNumber(input, setting);
        }
        body.append(setting.key, input.value);
      }
    });

    if (isLocalTest) {
      console.log('POST /settings (test mode):', body.toString());
      setStatus('Saved', 'ok');
      return;
    }

    setStatus('Saving…');
    fetch('/settings', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: body.toString()
    })
      .then(function (res) {
        if (!res.ok) throw new Error('HTTP ' + res.status);
        setStatus('Saved', 'ok');
      })
      .catch(function (err) {
        setStatus('Failed to save: ' + err.message, 'error');
      });
  });

  document.getElementById('reset-settings').addEventListener('click', function () {
    if (!confirm('Reset ALL settings to their defaults? This cannot be undone.')) return;

    if (isLocalTest) {
      console.log('POST /settings/reset (test mode)');
      setStatus('Settings reset to defaults', 'ok');
      return;
    }

    fetch('/settings/reset', { method: 'POST' })
      .then(function (res) {
        if (!res.ok) throw new Error('HTTP ' + res.status);
        load();
        setStatus('Settings reset to defaults', 'ok');
      })
      .catch(function (err) {
        setStatus('Reset failed: ' + err.message, 'error');
      });
  });

  load();
})();
