(function () {
  const statusEl = document.getElementById('status');
  // When served from 127.0.0.1 there is no webserver.
  const isLocalTest = location.hostname === '127.0.0.1';

  function setStatus(text, cls) {
    statusEl.textContent = text;
    statusEl.className = cls || '';
  }

  document.getElementById('start-turret').addEventListener('click', function () {
    if (isLocalTest) {
      console.log('POST /start (test mode)');
      setStatus('Start requested', 'ok');
      return;
    }
    fetch('/start', { method: 'POST' })
      .then(function (res) {
        if (!res.ok) throw new Error('HTTP ' + res.status);
        setStatus('Start requested', 'ok');
      })
      .catch(function (err) {
        setStatus('Failed to start: ' + err.message, 'error');
      });
  });
})();
