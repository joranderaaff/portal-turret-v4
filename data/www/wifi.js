(function () {
  const form = document.getElementById('wifi-form');
  const networksEl = document.getElementById('networks');
  const manualRadio = document.getElementById('network-manual');
  const manualSsid = document.getElementById('manual-ssid');
  const passwordEl = document.getElementById('wifi-password');
  const statusEl = document.getElementById('status');
  const rescanButton = document.getElementById('rescan');

  // When served from 127.0.0.1 there is no webserver, so use test data instead.
  const isLocalTest = location.hostname === '127.0.0.1';
  const TEST_NETWORKS = [
    { ssid: 'Home WiFi', rssi: -45, secure: true },
    { ssid: 'Neighbours', rssi: -78, secure: true },
    { ssid: 'Open Guest', rssi: -60, secure: false }
  ];

  const POLL_MS = 1500;
  let selectedSsid = '';
  let pollTimer = null;

  function setStatus(text, cls) {
    statusEl.textContent = text;
    statusEl.className = cls || '';
  }

  function signalLabel(rssi) {
    if (rssi >= -55) return 'excellent';
    if (rssi >= -67) return 'good';
    if (rssi >= -80) return 'fair';
    return 'weak';
  }

  function selectNetwork(ssid, isManual) {
    selectedSsid = ssid;
    manualSsid.hidden = !isManual;
    if (isManual) manualSsid.focus();
    else passwordEl.focus();
  }

  function renderNetworks(networks) {
    networksEl.textContent = '';
    if (networks.length === 0) {
      networksEl.textContent = 'No networks found.';
      return;
    }
    networks.sort(function (a, b) { return b.rssi - a.rssi; });
    networks.forEach(function (network) {
      const label = document.createElement('label');
      label.className = 'network';

      const radio = document.createElement('input');
      radio.type = 'radio';
      radio.name = 'network';
      radio.checked = network.ssid === selectedSsid;
      radio.addEventListener('change', function () { selectNetwork(network.ssid, false); });

      const name = document.createElement('span');
      name.textContent = network.ssid + (network.secure ? ' 🔒' : '');

      const signal = document.createElement('span');
      signal.className = 'hint';
      signal.textContent = signalLabel(network.rssi);

      label.appendChild(radio);
      label.appendChild(name);
      label.appendChild(signal);
      networksEl.appendChild(label);
    });
  }

  function scan(rescan) {
    clearTimeout(pollTimer);
    networksEl.textContent = 'Scanning…';
    rescanButton.disabled = true;
    poll(rescan);
  }

  function poll(rescan) {
    if (isLocalTest) {
      renderNetworks(TEST_NETWORKS.slice());
      rescanButton.disabled = false;
      return;
    }

    fetch('/wifi/scan' + (rescan ? '?rescan=1' : ''))
      .then(function (res) {
        if (!res.ok) throw new Error('HTTP ' + res.status);
        return res.json();
      })
      .then(function (data) {
        if (data.scanning) {
          pollTimer = setTimeout(poll, POLL_MS);
          return;
        }
        renderNetworks(data.networks);
        rescanButton.disabled = false;
      })
      .catch(function () {
        // The access point drops clients briefly while scanning; just retry.
        pollTimer = setTimeout(poll, POLL_MS);
      });
  }

  manualRadio.addEventListener('change', function () { selectNetwork(manualSsid.value, true); });
  manualSsid.addEventListener('input', function () { selectedSsid = manualSsid.value; });
  rescanButton.addEventListener('click', function () { scan(true); });

  form.addEventListener('submit', function (e) {
    e.preventDefault();

    const ssid = selectedSsid.trim() === '' ? '' : selectedSsid;
    if (ssid === '') {
      setStatus('Pick a network first', 'error');
      return;
    }

    const body = new URLSearchParams();
    body.append('ssid', ssid);
    body.append('password', passwordEl.value);

    if (isLocalTest) {
      console.log('POST /wifi/save (test mode):', body.toString());
      setStatus('Saved', 'ok');
      return;
    }

    setStatus('Saving…');
    fetch('/wifi/save', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: body.toString()
    })
      .then(function (res) {
        if (!res.ok) throw new Error('HTTP ' + res.status);
        setStatus('Saved. The turret restarts and joins "' + ssid +
          '". If it cannot connect, its "Portal Turret" network returns after 10 seconds.', 'ok');
      })
      .catch(function (err) {
        setStatus('Failed to save: ' + err.message, 'error');
      });
  });

  scan(false);
})();
