from pathlib import Path
from playwright.sync_api import sync_playwright
ROOT=Path(__file__).resolve().parents[1]
with sync_playwright() as p:
 b=p.chromium.launch(headless=True)
 page=b.new_page(viewport={'width':1280,'height':800})
 errors=[]
 page.on('pageerror',lambda e:errors.append(str(e)))
 page.set_content((ROOT/'index.html').read_text(encoding='utf-8'))
 page.add_style_tag(content=(ROOT/'css/style.css').read_text(encoding='utf-8'))
 page.evaluate("Object.defineProperty(window,'localStorage',{configurable:true,value:{_data:{},getItem(k){return this._data[k]||null},setItem(k,v){this._data[k]=v}}})")
 for script in ('data.js','renderer.js','game.js'):page.add_script_tag(content=(ROOT/'js'/script).read_text(encoding='utf-8'))
 page.locator('#startNew').click()
 assert page.locator('#skillGrid .skill').count()==130
 page.locator('#confirmSkills').click()
 assert page.evaluate('LAST_GAME.getState().level')==100
 for i in range(20):
  page.evaluate('LAST_GAME.spawnEnemy()')
  page.evaluate('LAST_GAME.debugVictory()')
  for j in range(6):
   if page.evaluate('LAST_GAME.getState().screen')!='loss':break
   page.evaluate('LAST_GAME.applyLoss(LAST_GAME.getState().innate[0])')
  assert page.evaluate('LAST_GAME.getState().screen')=='explore'
 assert page.evaluate('LAST_GAME.getState().level')==0
 page.evaluate('LAST_GAME.spawnEnemy()')
 assert page.evaluate('LAST_GAME.getState().enemy.mirror') is True
 page.evaluate('LAST_GAME.debugVictory()')
 assert page.evaluate('LAST_GAME.getState().screen')=='victory'
 assert not errors,errors
 print('PASS: full 100 -> 0 -> mirror -> victory browser state machine')
 b.close()
