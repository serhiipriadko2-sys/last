#!/usr/bin/env python3
from pathlib import Path
import hashlib,json,sys
root=Path(__file__).resolve().parents[1]
manifest=json.loads((root/'support/MANIFEST.json').read_text(encoding='utf-8'))
errors=[]
rows=manifest['files'];found=sorted((root/'knowledge').glob('[0-2][0-9]_*.md'))
if len(rows)!=30 or len(found)!=30:errors.append('Expected exactly thirty canonical Knowledge files')
for item in rows:
 p=root/item['path']; data=p.read_bytes() if p.is_file() and not p.is_symlink() else b''
 if len(data)!=item['bytes'] or hashlib.sha256(data).hexdigest()!=item['sha256']:errors.append('MISMATCH '+item['path'])
print('SoT30:', 'FAIL' if errors else 'PASS',len(rows),'files')
for e in errors:print(e)
sys.exit(bool(errors))
