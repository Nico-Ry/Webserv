function setOutput(meta, text) {
  const out = document.getElementById('out');
  out.querySelector('.meta').textContent = meta;
  out.querySelector('pre').textContent = text;
}

async function run(method, path, body, headers) {
  const t0 = performance.now();
  try {
	const opts = { method: method, headers: headers || {} };
	if (body !== undefined && body !== null) opts.body = body;

	const res = await fetch(path, opts);
	const t1 = performance.now();

	let text = '';
	try { text = await res.text(); } catch (_) { text = '[no body]'; }

	let headerDump = '';
	res.headers.forEach((v, k) => { headerDump += k + ': ' + v + '\n'; });

	const meta = method + ' ' + path + ' → ' + res.status + ' ' + res.statusText + ' (' + Math.round(t1 - t0) + 'ms)';
	setOutput(meta, headerDump + '\n' + text);
  } catch (e) {
	setOutput(method + ' ' + path + ' → ERROR', String(e));
  }
}

async function createDeleteTarget() {
  await run('POST', '/upload/to-delete.txt', 'delete-me\n', { 'Content-Type': 'text/plain' });
}

async function runBigNoSize() {
  let s = '';
  for (let i = 0; i < 1024; i++) s += 'A';
  await run('POST', '/no_size', s, { 'Content-Type': 'text/plain' });
}
