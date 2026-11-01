/* Last Light gameplay registry. Edit these numbers to rebalance without rebuilding. */
(function(){'use strict';
const C={version:'0.2.0',maxLevel:100,startingSkills:15,fastLevelsPerWin:5,normalXpBase:3000,normalXpMultiplier:.96,enemyDamageCap:32,attackTimeMs:1400,parryWindowMs:175,dodgeWindowMs:310,relaxedMultiplier:1.6,baseHp:125,baseAp:2,startingPotions:4,worldRadius:13,inventoryCap:24,saveKey:'lastlight.v02.save',equipmentBreak:{base:.10,perGrade:.12,max:.8},maxGrade:6};
const elements=[['FIRE','#ff7654','Ash'],['FROST','#9ccaf8','Frost'],['STORM','#e8c773','Thunder'],['VENOM','#a8d777','Venom'],['STONE','#cda98e','Stone'],['TIDE','#73cccd','Tide'],['GALE','#b3d7a5','Gale'],['IRON','#c8c0b3','Iron'],['VOID','#a29be3','Void'],['HALO','#f6ddaa','Dawn']];
const bases=[
['Strike','ATTACK',1,16,'A precise single-target blow.'],['Arc','ATTACK',2,26,'A sweeping elemental attack.'],
['Lance','ATTACK',2,24,'A piercing strike that weakens armor.'],['Ward','DEFENSE',1,0,'Reduce the next incoming hit.'],
['Surge','ATTACK',2,18,'Attack and recover resolve.'],['Pulse','SUPPORT',1,10,'Restore a little vitality.'],
['Fang','ATTACK',1,14,'A swift strike, improves critical chance.'],['Rift','ATTACK',2,22,'Leaves a lingering affliction.'],
['Vow','SUPPORT',1,0,'Raise your next attack.'],['Echo','ATTACK',1,12,'Two lighter impacts.']];
let skills=[];for(let e=0;e<elements.length;e++)for(let b=0;b<bases.length;b++){
let a=bases[b],el=elements[e],id='S'+String(e*10+b+1).padStart(3,'0');
skills.push({id,name:el[2]+' '+a[0],element:el[0],color:el[1],type:a[1],ap:a[2],power:a[3]+(e%3)*2,grade:1,desc:a[4],effect:b===3?'guard':b===5?'heal':b===8?'empower':b===2?'armor':b===7?'dot':b===4?'resolve':b===6?'crit':b===9?'double':'damage'});
}
const authored=[
['Crown of Dust','VOID','DEFENSE','guard'],['Last Ember','FIRE','ATTACK','dot'],['Cold Memory','FROST','SUPPORT','heal'],['Judgement Bell','HALO','ATTACK','armor'],['Thousand Cuts','IRON','ATTACK','double'],['Drowned Prayer','TIDE','SUPPORT','heal'],['Storm Hollow','STORM','ATTACK','damage'],['Silent Thorn','VENOM','ATTACK','dot'],['Ashen Oath','FIRE','SUPPORT','empower'],['Steel Testament','IRON','DEFENSE','guard'],
['Ghost Lantern','VOID','ATTACK','crit'],['Root of the World','STONE','DEFENSE','guard'],['Breath of Kings','GALE','SUPPORT','empower'],['Dusk Anvil','STONE','ATTACK','armor'],['Siren Skin','TIDE','DEFENSE','guard'],['Holy Rupture','HALO','ATTACK','double'],['Rain of Needles','STORM','ATTACK','dot'],['The First Scar','IRON','ATTACK','crit'],['Whispering Ice','FROST','ATTACK','dot'],['Toxic Benediction','VENOM','SUPPORT','heal'],
['Ruin Promise','VOID','ATTACK','armor'],['Dawn Without Name','HALO','SUPPORT','heal'],['Sundered Horizon','GALE','ATTACK','damage'],['Fever Dream','FIRE','ATTACK','double'],['Glacial Heart','FROST','DEFENSE','guard'],['Falling Citadel','STONE','ATTACK','crit'],['Ocean of Mirrors','TIDE','ATTACK','double'],['Hunger for Light','VOID','SUPPORT','empower'],['Saint of Sparks','STORM','ATTACK','crit'],['After the End','HALO','ATTACK','damage']];
for(let i=0;i<authored.length;i++){let a=authored[i],el=elements.find(e=>e[0]===a[1]);skills.push({id:'U'+String(i+1).padStart(3,'0'),name:a[0],element:a[1],color:el[1],type:a[2],effect:a[3],power:20+(i%7)*4,ap:(a[2]==='DEFENSE'||a[2]==='SUPPORT')?1:2,grade:2,desc:'Unique legacy art. '+(a[3]==='dot'?'Inflicts a lasting wound.':a[3]==='heal'?'Restores vitality.':'Changes the rhythm of battle.')});}
const families=[['Ash Warden','KNIGHT',0],['Pale Stalker','BEAST',1],['Glass Seraph','WRAITH',2],['Iron Penitent','GOLEM',3],['Thorn Reaver','SCOUT',4],['Sunless Acolyte','MAGE',5]];
const traits=[['of Cinders','FIRE'],['of Silence','VOID'],['of Winter','FROST'],['of Tempests','STORM'],['of the Deep','TIDE']];
const enemies=[];for(let f=0;f<families.length;f++)for(let t=0;t<traits.length;t++){
let fam=families[f],trait=traits[t],el=elements.find(e=>e[0]===trait[1]);
enemies.push({id:'E'+String(f*5+t+1).padStart(2,'0'),name:fam[0]+' '+trait[0],kind:fam[1],element:trait[1],color:el[1],hp:76+f*14+t*5,damage:11+f*2+t,tempo:1.35+f*.07,weakTo:elements[(t*2+1)%10][0],pattern:['slash','stab','slam','flare','cleave'][(f+t)%5]});
}
const miniNames=['The Withered Monarch','The Keeper of Names','The Brass Pilgrim','The Choir of Ash','The Broken Architect','The Winter Witness','The Empty Scholar','The Unburied Saint','The Blacksmith of Time'];
const minibosses=miniNames.map((name,i)=>({id:'M'+(i+1),name,kind:['KNIGHT','WRAITH','GOLEM'][i%3],element:elements[(i*3)%10][0],color:elements[(i*3)%10][1],hp:190+i*21,damage:18+i*2,tempo:1.1+i*.05,weakTo:elements[(i*3+4)%10][0],pattern:['slash','flare','slam'][i%3],elite:true,threshold:90-i*10}));
const itemNames=['Shattered','Forgotten','Sunken','Hollow','Chiming','Oathbound','Ancient','Silent','Gilded','Mournful'];
const itemNouns=['Greatblade','Lantern','Mantle','Sigil','Crown','Gauntlet','Rosary','Talisman','Bastion','Banner'];
const rarityNames=['COMMON','RARE','RELIC'];
const skillById={};for(const s of skills)skillById[s.id]=s;
function expToLose(level){return Math.max(1,Math.round(C.normalXpBase*Math.pow(C.normalXpMultiplier,100-Math.max(0,level))))}
function breakChance(grade){return Math.min(C.equipmentBreak.max,C.equipmentBreak.base+C.equipmentBreak.perGrade*(Math.max(1,grade)-1))}
function enemyScaling(level){const lost=(100-level)/100;return {health:1+lost*.66,damage:1+lost*.65,attacks:1+(lost>.6?1:0)+(lost>.9?1:0),window:1-lost*.38}}
function generateItem(level,rng=Math.random){let rarity=rng()<.08?3:rng()<.27?2:1;let base=Math.floor(rng()*itemNouns.length);let count=rarity;let ids=[];for(let i=0;i<count;i++){let a=skills[Math.floor(rng()*skills.length)].id;if(!ids.includes(a))ids.push(a)}return {id:'I'+Date.now().toString(36)+Math.floor(rng()*1e7).toString(36),name:itemNames[Math.floor(rng()*itemNames.length)]+' '+itemNouns[base],grade:1,rarity,slot:(base%2===0?'WEAPON':'RELIC'),skills:ids,power:6+Math.floor((100-level)/15)+rarity*4,seal:rarity===3&&rng()<.5};}
window.LAST_DATA={C,elements,bases,skills,skillById,enemies,minibosses,rarityNames,generateItem,expToLose,breakChance,enemyScaling};
})();
