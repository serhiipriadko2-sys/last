from pathlib import Path
from playwright.sync_api import sync_playwright
ROOT=Path(__file__).resolve().parents[1]
with sync_playwright() as p:
 b=p.chromium.launch(headless=True)
 page=b.new_page(viewport={'width':1280,'height':800})
 page.set_content((ROOT/'index.html').read_text(encoding='utf-8'))
 page.add_style_tag(content=(ROOT/'css/style.css').read_text(encoding='utf-8'))
 page.evaluate("Object.defineProperty(window,'localStorage',{configurable:true,value:{_data:{},getItem(k){return this._data[k]||null},setItem(k,v){this._data[k]=v}}})")
 for s in ('data.js','renderer.js','game.js'):page.add_script_tag(content=(ROOT/'js'/s).read_text(encoding='utf-8'))
 page.locator('#startNew').click();page.locator('#confirmSkills').click()
 page.evaluate('LAST_GAME.spawnEnemy()')
 original=page.evaluate('LAST_GAME.getState().hp')
 page.evaluate('LAST_GAME.endTurn()')
 page.evaluate('LAST_GAME.getState().defenseStart=performance.now()-LAST_GAME.getState().hitRatio*LAST_GAME.getState().defenseDuration')
 page.evaluate('LAST_GAME.defenseInput("parry")')
 assert page.evaluate('LAST_GAME.getState().hp')==original
 assert page.evaluate('LAST_GAME.getState().resolve')>=2
 page.wait_for_timeout(750)
 page.evaluate('LAST_GAME.debugVictory()')
 for i in range(5):
  if page.evaluate('LAST_GAME.getState().screen')=='loss':
   page.evaluate('LAST_GAME.applyLoss(LAST_GAME.getState().innate[0])')
 assert page.evaluate('LAST_GAME.getState().level')==95
 assert page.evaluate('LAST_GAME.getState().innate.length')==10
 assert page.evaluate('LAST_GAME.getState().evolutionCharges')==1
 page.evaluate('LAST_GAME.showInventory()')
 page.locator('#evolveItems .inventory-card').nth(0).locator('input').check()
 page.locator('#evolveItems .inventory-card').nth(1).locator('input').check()
 page.locator('#evolveBtn').click()
 assert page.evaluate('LAST_GAME.getState().evolutionCharges')==0
 assert any('parents' in item for item in page.evaluate('Object.values(LAST_GAME.getState().fusionSkills)'))
 print('PASS: parry, level loss, evolution')
 b.close()
