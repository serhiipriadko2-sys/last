/* Tiny dependency-free perspective renderer: actual 3D solids projected into Canvas2D. */
(function(){'use strict';
const clamp=(v,a,b)=>Math.max(a,Math.min(b,v));
const vsub=(a,b)=>[a[0]-b[0],a[1]-b[1],a[2]-b[2]];
const dot=(a,b)=>a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
const cross=(a,b)=>[a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]];
const norm=(a)=>{let m=Math.sqrt(dot(a,a))||1;return a.map(x=>x/m)};
const LIGHT=norm([-.7,1.2,.4]);
function rgb(hex){const n=parseInt(hex.slice(1),16);return [(n>>16)&255,(n>>8)&255,n&255]}
function shade(color,light,depth){const c=rgb(color||'#a19d92');const fog=clamp((depth-7)/32,0,.72);let m=clamp(.27+light*.66,.12,1.25);let out=c.map(x=>Math.floor(x*m*(1-fog)+[15,23,36][c.indexOf(x)]*fog));return 'rgb('+out.join(',')+')'}
class SceneRenderer{
 constructor(canvas){this.canvas=canvas;this.ctx=canvas.getContext('2d',{alpha:false});this.resize();window.addEventListener('resize',()=>this.resize());this.polys=[];this.cam=null;this.seed=13}
 resize(){const dpr=Math.min(2,window.devicePixelRatio||1);this.w=Math.round(window.innerWidth*dpr);this.h=Math.round(window.innerHeight*dpr);this.canvas.width=this.w;this.canvas.height=this.h;this.scale=dpr;}
 camera(eye,target){const forward=norm(vsub(target,eye)),right=norm(cross(forward,[0,1,0])),up=norm(cross(right,forward));this.cam={eye,forward,right,up,fov:Math.min(this.w,this.h)*.96};}
 project(p){const rel=vsub(p,this.cam.eye),z=dot(rel,this.cam.forward);if(z<.16)return null;return [this.w*.5+dot(rel,this.cam.right)*this.cam.fov/z,this.h*.54-dot(rel,this.cam.up)*this.cam.fov/z,z]}
 addPoly(points,color,alpha=1){const avg=points.reduce((a,p)=>[a[0]+p[0],a[1]+p[1],a[2]+p[2]],[0,0,0]).map(v=>v/points.length);const p=points.map(v=>this.project(v));if(p.some(v=>!v))return;const A=vsub(points[1],points[0]),B=vsub(points[2],points[0]),normal=norm(cross(A,B));const l=Math.abs(dot(normal,LIGHT));this.polys.push({points:p,depth:p.reduce((sum,q)=>sum+q[2],0)/p.length,color:shade(color,l,dot(vsub(avg,this.cam.eye),this.cam.forward)),alpha});}
 box(x,y,z,sx,sy,sz,col,ry=0,rx=0){const hx=sx/2,hy=sy/2,hz=sz/2;let corners=[[-hx,-hy,-hz],[hx,-hy,-hz],[hx,hy,-hz],[-hx,hy,-hz],[-hx,-hy,hz],[hx,-hy,hz],[hx,hy,hz],[-hx,hy,hz]];let ca=Math.cos(ry),sa=Math.sin(ry),cx=Math.cos(rx),sn=Math.sin(rx);corners=corners.map(p=>{let q=[p[0],p[1]*cx-p[2]*sn,p[1]*sn+p[2]*cx];return [x+q[0]*ca-q[2]*sa,y+q[1],z+q[0]*sa+q[2]*ca]});for(const idx of [[0,3,2,1],[4,5,6,7],[0,4,7,3],[1,2,6,5],[3,7,6,2],[0,1,5,4]])this.addPoly(idx.map(i=>corners[i]),col);}
 pyramid(x,y,z,size,height,col){const a=[[x-size,y,z-size],[x+size,y,z-size],[x+size,y,z+size],[x-size,y,z+size]],top=[x,y+height,z];this.addPoly([a[0],a[1],top],col);this.addPoly([a[1],a[2],top],col);this.addPoly([a[2],a[3],top],col);this.addPoly([a[3],a[0],top],col);}
 disk(x,y,z,r,color,segments=12){let arr=[];for(let i=0;i<segments;i++){let a=i*Math.PI*2/segments;arr.push([x+Math.cos(a)*r,y,z+Math.sin(a)*r])}this.addPoly(arr,color,.55)}
 humanoid(x,z,opts={}){const t=opts.time||0, moving=opts.moving, attack=opts.attack||0, dark=opts.dark, boss=opts.boss, scale=opts.scale||1;let y=opts.y||0;const skin=boss?'#eddcaa':dark?'#8b9fbb':'#c3b8a4',armor=boss?'#e1b36d':dark?'#394453':'#60777e',trim=boss?'#fff0be':'#be9271',size=scale;let sway=Math.sin(t*2.3)*.045,walk=moving?Math.sin(t*12)*.3:0,arm=attack>0?Math.sin((1-attack)*Math.PI)*1.1:0;y+=Math.sin(t*2.2)*.04;
 this.disk(x,.025,z,.74*size,'#0c1017');
 this.box(x,y+.47*size,z-.02*size,.37*size,.82*size,.38*size,armor,.08,sway);
 this.box(x,y+1.11*size,z,.57*size,.55*size,.42*size,armor,0,-sway);
 this.box(x,y+1.64*size,z,.32*size,.35*size,.31*size,skin,0,0);
 this.box(x,y+1.83*size,z,.42*size,.17*size,.34*size,trim,0,-.12);
 this.box(x-.15*size,y+.18*size,z,.18*size,.49*size,.22*size,skin,0,walk);
 this.box(x+.15*size,y+.18*size,z,.18*size,.49*size,.22*size,skin,0,-walk);
 this.box(x-.39*size,y+.92*size,z,.20*size,.66*size,.23*size,skin,0,-walk*.4);
 this.box(x+.39*size,y+.98*size,z,.22*size,.64*size,.24*size,skin,0,-.5-arm);
 this.box(x+.62*size,y+1.01*size,z+.23*size,.095*size,1.13*size,.10*size,trim,.1,-.6-arm);
 if(boss){this.pyramid(x,y+2.02*size,z,.25*size,.32*size,'#dfb466');this.disk(x,y+.12,z,1.2*size,'#82603c')}
 }
 beast(x,z,opts={}){const t=opts.time||0,col=opts.color||'#9e7d75',scale=opts.scale||1,kind=opts.kind||'BEAST',attack=opts.attack||0;let y=Math.sin(t*2.1)*.045;this.disk(x,.025,z,.94*scale,'#0e1019');if(kind==='WRAITH'){this.pyramid(x,y+.3,z,.66*scale,1.65*scale,col);this.pyramid(x,y+1.65,z,.34*scale,.5*scale,'#d2bda9');this.box(x,y+1.47,z+.34*scale,.30*scale,.05*scale,.10*scale,'#f3e0cf');return}
 if(kind==='GOLEM'){this.box(x,y+1.04*scale,z,1.10*scale,1.33*scale,.72*scale,col,0,0);this.box(x,y+2.05*scale,z,.60*scale,.53*scale,.55*scale,'#9eaaa8');for(let s of [-1,1]){this.box(x+s*.85*scale,y+1.12*scale,z,.51*scale,1.12*scale,.51*scale,col,0,Math.sin(t*2)*.12);this.box(x+s*.34*scale,y+.3*scale,z,.44*scale,.76*scale,.48*scale,'#5a6266')}return}
 if(kind==='BEAST'){this.box(x,y+.84*scale,z,1.12*scale,.64*scale,.66*scale,col);this.box(x,y+1.05*scale,z+.55*scale,.65*scale,.53*scale,.63*scale,'#b9a594');for(let s of [-1,1]){this.pyramid(x+s*.24*scale,y+1.34*scale,z+.7*scale,.17*scale,.35*scale,'#d6a578')}for(let s of [-1,1])for(let d of [-1,1])this.box(x+s*.40*scale,y+.4*scale,z+d*.25*scale,.22*scale,.65*scale,.24*scale,col,0,Math.sin(t*6+s+d)*.08);return}
 this.humanoid(x,z,{time:t,dark:true,scale:scale,boss:false,attack});if(kind==='MAGE'){this.pyramid(x,y+1.54*scale,z,.54*scale,.59*scale,'#705f82');this.box(x-.78*scale,y+.8*scale,z,.10*scale,1.7*scale,.10*scale,'#ceb3a3')}
 }
 torches(t){for(let i=0;i<8;i++){let a=i*Math.PI/4,rad=11.1,x=Math.cos(a)*rad,z=Math.sin(a)*rad;this.box(x,1.6,z,.35,3.3,.35,'#66615c');this.pyramid(x,3.1+Math.sin(t*7+i)*.17,z,.34,.85,'#ec9a55');this.pyramid(x,3.31+Math.sin(t*8+i)*.10,z,.19,.45,'#f6db9c');}}
 ground(t){this.box(0,-.4,0,30,.65,30,'#303b3d');for(let ix=-6;ix<=6;ix++)for(let iz=-6;iz<=6;iz++){const x=ix*2.15+(iz%2)*.3,z=iz*2.15;if(Math.hypot(x,z)>14.4)continue;let h=.03+((ix*13+iz*17+983)%5)*.009;let col=((ix*11+iz*7)&3)===0?'#434e50':'#39484a';this.box(x,-.04,z,2.08,h,2.08,col,0)}
 for(let i=0;i<26;i++){const a=i*2.399963,rad=13.5+(i%4)*1.3,x=Math.cos(a)*rad,z=Math.sin(a)*rad;this.box(x,.45,z,1.2,.95,1.2,'#41444b',a*.3);if(i%3===0){this.box(x,2.05,z,.72,2.5,.72,'#555659',a*.4);this.box(x,3.27,z,1.5,.20,1.5,'#6c665e',a*.4)}else if(i%5===0)this.pyramid(x,1.0,z,1.4,2.3,'#535b56')}
 this.torches(t);
 }
 portal(x,z,t,active){for(let i=0;i<12;i++){let a=(i/12)*Math.PI*2+t*.08;let xx=x+Math.cos(a)*1.35,yy=2.1+Math.sin(a)*1.65;this.box(xx,yy,z,.31,.55,.30,active?'#e4b978':'#61686d',a)}if(active){this.box(x,2.1,z,.72,2.7,.12,'#94bccc');this.pyramid(x,3.34,z,.75,.55,'#faf3bb')}}
 world(state,t){const c=this.ctx,w=this.w,h=this.h;let sky=c.createLinearGradient(0,0,0,h);sky.addColorStop(0,'#101c2f');sky.addColorStop(.5,'#2b3b4c');sky.addColorStop(.89,'#aa8266');sky.addColorStop(1,'#242b35');c.fillStyle=sky;c.fillRect(0,0,w,h);
 c.fillStyle='#d7b58d';c.shadowColor='#d8ab77';c.shadowBlur=70*this.scale;c.beginPath();c.arc(w*.74,h*.21,Math.min(w,h)*.065,0,Math.PI*2);c.fill();c.shadowBlur=0;
 this.polys.length=0;let battle=state.screen==='battle'||state.screen==='defense',isMenu=state.screen==='title'||state.screen==='select';let camx=state.pos.x+9,camz=state.pos.z+13; if(battle){this.camera([8,5.1,13.2],[0,1,-.4])}else if(isMenu){this.camera([10,6.3,17],[0,1,0])}else{this.camera([camx,7.1,camz],[state.pos.x,1.0,state.pos.z])}this.ground(t);
 if(isMenu){this.humanoid(-2,2,{time:t,scale:1.35,boss:true});this.beast(2.2,-2,{time:t,kind:'WRAITH',color:'#899bb7',scale:1.1});this.portal(0,-6,t,true)}
 else if(battle){this.humanoid(-2.5,2.4,{time:t,moving:false,attack:state.attackFx?.hero||0});if(state.enemy){const e=state.enemy;if(e.mirror){this.humanoid(2.4,-2,{time:t+1,boss:true,scale:1.6,attack:state.attackFx?.enemy||0})}else this.beast(2.4,-2,{time:t,kind:e.kind,color:e.color,scale:e.elite?1.4:1.08,attack:state.attackFx?.enemy||0})}this.portal(0,-8,t,state.level===0)}
 else{this.humanoid(state.pos.x,state.pos.z,{time:t,moving:state.moving,attack:state.attackFx?.hero||0});this.portal(0,-9,t,state.level===0);this.disk(3,.07,3,1,'#655d37');this.box(3,.55,3,1.25,1.08,.95,'#846945');this.box(3,1.17,3,1.4,.16,1.06,'#c8a778');this.pyramid(3,1.23,3,.25,.75,'#dab579');if(state.level>0){this.beast(-3.5,-3.5,{time:t,kind:state.nextEnemyKind||'KNIGHT',color:'#b87e73',scale:1.12});this.disk(-3.5,.04,-3.5,1.4,'#755247')}}
 this.polys.sort((a,b)=>b.depth-a.depth);for(const face of this.polys){if(face.points.every(p=>p[0]<-w||p[0]>2*w||p[1]<-h||p[1]>2*h))continue;c.globalAlpha=face.alpha;c.beginPath();face.points.forEach((p,i)=>i?c.lineTo(p[0],p[1]):c.moveTo(p[0],p[1]));c.closePath();c.fillStyle=face.color;c.fill();}c.globalAlpha=1;
 let shadeBottom=c.createLinearGradient(0,h*.5,0,h);shadeBottom.addColorStop(0,'#0000');shadeBottom.addColorStop(1,'#070a13bb');c.fillStyle=shadeBottom;c.fillRect(0,h*.5,w,h*.5);
 if(state.fx){const now=performance.now();state.fx=state.fx.filter(f=>now-f.at<1100);for(const f of state.fx){let p=this.project([f.x,f.y+(now-f.at)*.0015,f.z]);if(!p)continue;c.globalAlpha=clamp(1-(now-f.at)/1100,0,1);c.textAlign='center';c.font=`bold ${Math.round(24*this.scale)}px Georgia`;c.fillStyle=f.color;c.fillText(f.text,p[0],p[1]);}c.globalAlpha=1}
 }
}
window.LastRenderer=SceneRenderer;
})();
