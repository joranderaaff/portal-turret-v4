// Fills <header id="site-nav"> from nav.json so every page shares one menu.
// Add a page by adding { "label", "href" } to nav.json.
(function () {
  const header = document.getElementById('site-nav');
  if (!header) {
    return;
  }
  const current = location.pathname.split('/').pop() || 'index.html';

  fetch('nav.json')
    .then(function (response) { return response.json(); })
    .then(function (items) {
      const nav = document.createElement('nav');
      items.forEach(function (item) {
        const link = document.createElement('a');
        link.href = item.href;
        link.textContent = item.label;
        if (item.href === current) {
          link.className = 'active';
          link.setAttribute('aria-current', 'page');
        }
        nav.appendChild(link);
      });
      header.appendChild(nav);
    })
    .catch(function () { /* header simply stays empty */ });
})();
