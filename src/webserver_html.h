#ifndef WEBSERVER_HTML_H
#define WEBSERVER_HTML_H

// HTML字符串片段，避免在原始字符串中使用中文字符导致的编码问题

const char HTML_HEAD[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>手冲咖啡称重计时器</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
:root{
  --primary:#6366f1;
  --primary-dark:#4f46e5;
  --secondary:#8b5cf6;
  --accent:#ec4899;
  --success:#10b981;
  --warning:#f59e0b;
  --danger:#ef4444;
  --info:#3b82f6;
  --bg-dark:#0f172a;
  --bg-card:#1e293b;
  --text-primary:#f1f5f9;
  --text-secondary:#94a3b8;
  --border:#334155;
  --shadow:0 20px 25px -5px rgba(0,0,0,0.3),0 10px 10px -5px rgba(0,0,0,0.2);
  --shadow-lg:0 25px 50px -12px rgba(0,0,0,0.5);
}
body{
  font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,'Helvetica Neue',Arial,sans-serif;
  background:linear-gradient(135deg,#0f172a 0%,#1e293b 50%,#334155 100%);
  min-height:100vh;
  padding:20px;
  color:var(--text-primary);
  position:relative;
  overflow-x:hidden;
}
body::before{
  content:'';
  position:fixed;
  top:0;
  left:0;
  right:0;
  bottom:0;
  background:radial-gradient(circle at 20% 50%,rgba(99,102,241,0.1) 0%,transparent 50%),
              radial-gradient(circle at 80% 80%,rgba(139,92,246,0.1) 0%,transparent 50%);
  pointer-events:none;
  z-index:0;
}
.container{
  max-width:700px;
  margin:0 auto;
  position:relative;
  z-index:1;
}
.header{
  background:linear-gradient(135deg,var(--primary) 0%,var(--secondary) 100%);
  color:white;
  padding:35px 30px;
  text-align:center;
  border-radius:20px 20px 0 0;
  box-shadow:var(--shadow);
  position:relative;
  overflow:hidden;
}
.header::before{
  content:'';
  position:absolute;
  top:-50%;
  left:-50%;
  width:200%;
  height:200%;
  background:radial-gradient(circle,rgba(255,255,255,0.1) 0%,transparent 70%);
  animation:rotate 20s linear infinite;
}
@keyframes rotate{
  from{transform:rotate(0deg)}
  to{transform:rotate(360deg)}
}
.header h1{
  font-size:28px;
  margin-bottom:8px;
  position:relative;
  z-index:1;
  text-shadow:0 2px 10px rgba(0,0,0,0.3);
}
.header p{
  opacity:0.95;
  font-size:15px;
  position:relative;
  z-index:1;
}
.content{
  background:var(--bg-card);
  padding:30px;
  border-radius:0 0 20px 20px;
  box-shadow:var(--shadow-lg);
  backdrop-filter:blur(10px);
}
.section{
  margin-bottom:35px;
  animation:fadeIn 0.5s ease-out;
}
@keyframes fadeIn{
  from{opacity:0;transform:translateY(20px)}
  to{opacity:1;transform:translateY(0)}
}
.section-title{
  font-size:20px;
  font-weight:700;
  margin-bottom:20px;
  color:var(--text-primary);
  display:flex;
  align-items:center;
  gap:10px;
  padding-bottom:12px;
  border-bottom:2px solid var(--border);
}
.section-title::before{
  content:'';
  width:4px;
  height:20px;
  background:linear-gradient(180deg,var(--primary),var(--secondary));
  border-radius:2px;
}
.card{
  background:rgba(30,41,59,0.6);
  backdrop-filter:blur(10px);
  border:1px solid var(--border);
  border-radius:16px;
  padding:30px;
  margin-bottom:20px;
  box-shadow:0 8px 32px rgba(0,0,0,0.2);
  transition:all 0.3s ease;
}
.card:hover{
  transform:translateY(-2px);
  box-shadow:0 12px 40px rgba(0,0,0,0.3);
}
.weight-display{
  text-align:center;
  padding:40px 30px;
  background:linear-gradient(135deg,rgba(99,102,241,0.1) 0%,rgba(139,92,246,0.1) 100%);
  border-radius:20px;
  border:1px solid rgba(99,102,241,0.2);
  position:relative;
  overflow:hidden;
}
.weight-display::before{
  content:'';
  position:absolute;
  top:0;
  left:0;
  right:0;
  height:3px;
  background:linear-gradient(90deg,var(--primary),var(--secondary),var(--accent));
  animation:shimmer 3s ease-in-out infinite;
}
@keyframes shimmer{
  0%,100%{opacity:0.5}
  50%{opacity:1}
}
.weight-value{
  font-size:64px;
  font-weight:700;
  background:linear-gradient(135deg,var(--text-primary) 0%,var(--text-secondary) 100%);
  -webkit-background-clip:text;
  -webkit-text-fill-color:transparent;
  background-clip:text;
  margin-bottom:8px;
  transition:all 0.3s ease;
  font-variant-numeric:tabular-nums;
}
.weight-value.updated{
  animation:pulse 0.3s ease;
}
@keyframes pulse{
  0%,100%{transform:scale(1)}
  50%{transform:scale(1.05)}
}
.weight-unit{
  font-size:24px;
  color:var(--text-secondary);
  font-weight:500;
}
.ratio-display{
  font-size:16px;
  color:var(--accent);
  margin-top:15px;
  font-weight:600;
  padding:8px 16px;
  background:rgba(236,72,153,0.1);
  border-radius:20px;
  display:inline-block;
}
.timer-display{
  text-align:center;
  padding:35px 30px;
  background:linear-gradient(135deg,rgba(239,68,68,0.1) 0%,rgba(245,158,11,0.1) 100%);
  border-radius:20px;
  border:1px solid rgba(239,68,68,0.2);
  position:relative;
}
.timer-value{
  font-size:56px;
  font-weight:700;
  font-family:'Courier New',monospace;
  letter-spacing:4px;
  color:var(--danger);
  text-shadow:0 0 20px rgba(239,68,68,0.5);
  font-variant-numeric:tabular-nums;
  transition:all 0.3s ease;
}
.timer-value.running{
  animation:glow 2s ease-in-out infinite;
}
@keyframes glow{
  0%,100%{text-shadow:0 0 20px rgba(239,68,68,0.5)}
  50%{text-shadow:0 0 30px rgba(239,68,68,0.8),0 0 40px rgba(239,68,68,0.4)}
}
.button-group{
  display:grid;
  grid-template-columns:repeat(auto-fit,minmax(120px,1fr));
  gap:12px;
  margin-bottom:20px;
}
.btn{
  padding:14px 24px;
  border:none;
  border-radius:12px;
  font-size:15px;
  font-weight:600;
  cursor:pointer;
  transition:all 0.3s cubic-bezier(0.4,0,0.2,1);
  color:white;
  position:relative;
  overflow:hidden;
  box-shadow:0 4px 6px rgba(0,0,0,0.2);
}
.btn::before{
  content:'';
  position:absolute;
  top:50%;
  left:50%;
  width:0;
  height:0;
  border-radius:50%;
  background:rgba(255,255,255,0.3);
  transform:translate(-50%,-50%);
  transition:width 0.6s,height 0.6s;
}
.btn:active::before{
  width:300px;
  height:300px;
}
.btn-primary{background:linear-gradient(135deg,var(--primary),var(--primary-dark))}
.btn-success{background:linear-gradient(135deg,var(--success),#059669)}
.btn-danger{background:linear-gradient(135deg,var(--danger),#dc2626)}
.btn-warning{background:linear-gradient(135deg,var(--warning),#d97706)}
.btn-info{background:linear-gradient(135deg,var(--info),#2563eb)}
.btn:hover{
  transform:translateY(-3px);
  box-shadow:0 8px 15px rgba(0,0,0,0.3);
}
.btn:active{
  transform:translateY(-1px);
}
.input-group{
  margin-bottom:20px;
}
.input-group label{
  display:block;
  margin-bottom:8px;
  font-weight:600;
  color:var(--text-primary);
  font-size:14px;
}
.input-group input,.input-group select{
  width:100%;
  padding:14px 18px;
  border:2px solid var(--border);
  border-radius:12px;
  font-size:15px;
  background:rgba(15,23,42,0.5);
  color:var(--text-primary);
  transition:all 0.3s ease;
  backdrop-filter:blur(10px);
}
.input-group input:focus,.input-group select:focus{
  outline:none;
  border-color:var(--primary);
  box-shadow:0 0 0 3px rgba(99,102,241,0.2);
  background:rgba(15,23,42,0.7);
}
.input-group input::placeholder{
  color:var(--text-secondary);
}
.segments{
  margin-top:20px;
}
.segment-item{
  padding:16px 20px;
  background:rgba(15,23,42,0.4);
  border:1px solid var(--border);
  border-radius:12px;
  margin-bottom:12px;
  display:flex;
  justify-content:space-between;
  align-items:center;
  transition:all 0.3s ease;
  backdrop-filter:blur(10px);
}
.segment-item:hover{
  background:rgba(15,23,42,0.6);
  transform:translateX(5px);
  border-color:var(--primary);
}
.segment-name{
  font-weight:600;
  color:var(--text-primary);
  font-size:15px;
}
.segment-time{
  color:var(--text-secondary);
  font-family:'Courier New',monospace;
  font-size:16px;
  font-weight:600;
  padding:6px 12px;
  background:rgba(99,102,241,0.1);
  border-radius:8px;
}
.status{
  padding:14px 20px;
  border-radius:12px;
  margin-bottom:15px;
  text-align:center;
  font-weight:600;
  font-size:15px;
  box-shadow:0 4px 6px rgba(0,0,0,0.2);
  animation:slideDown 0.3s ease;
}
@keyframes slideDown{
  from{opacity:0;transform:translateY(-20px)}
  to{opacity:1;transform:translateY(0)}
}
.status.success{
  background:linear-gradient(135deg,rgba(16,185,129,0.2),rgba(16,185,129,0.1));
  color:#10b981;
  border:1px solid rgba(16,185,129,0.3);
}
.status.error{
  background:linear-gradient(135deg,rgba(239,68,68,0.2),rgba(239,68,68,0.1));
  color:#ef4444;
  border:1px solid rgba(239,68,68,0.3);
}
.empty-state{
  text-align:center;
  color:var(--text-secondary);
  padding:40px 20px;
  font-size:15px;
}
.progress-bar{
  width:100%;
  height:6px;
  background:rgba(255,255,255,0.1);
  border-radius:3px;
  overflow:hidden;
  margin-top:15px;
}
.progress-fill{
  height:100%;
  background:linear-gradient(90deg,var(--primary),var(--secondary));
  border-radius:3px;
  transition:width 0.3s ease;
  box-shadow:0 0 10px rgba(99,102,241,0.5);
}
@media (max-width:600px){
  .container{max-width:100%}
  .content{padding:20px}
  .weight-value{font-size:48px}
  .timer-value{font-size:42px;letter-spacing:2px}
  .button-group{grid-template-columns:repeat(2,1fr)}
}
</style>
</head>
<body>
<div class="container">
<div class="header">
<h1>☕ 手冲咖啡称重计时器</h1>
<p>实时称重 &amp; 分段计时</p>
</div>
<div class="content">
<div class="section">
<div class="section-title">⚖️ 重量</div>
<div class="card">
<div class="weight-display">
<div class="weight-value" id="weight">0.0</div>
<div class="weight-unit">g</div>
<div class="ratio-display" id="ratio"></div>
</div>
<button class="btn btn-primary" onclick="tare()">去皮</button>
</div>
</div>
<div class="section">
<div class="section-title">⏱️ 计时器</div>
<div class="card">
<div class="timer-display">
<div class="timer-value" id="timer">00:00.000</div>
</div>
<div class="button-group">
<button class="btn btn-success" onclick="startTimer()">开始</button>
<button class="btn btn-warning" onclick="pauseTimer()">暂停</button>
<button class="btn btn-danger" onclick="stopTimer()">停止</button>
<button class="btn btn-info" onclick="resetTimer()">重置</button>
</div>
<div class="input-group">
<input type="text" id="segmentName" placeholder="输入分段名称（如：闷蒸、第一段注水等）">
<button class="btn btn-primary" onclick="addSegment()" style="margin-top:12px;width:100%">添加分段</button>
</div>
<div class="segments" id="segments"></div>
</div>
</div>
<div class="section">
<div class="section-title">⚙️ 设置</div>
<div class="card">
<div class="input-group">
<label>咖啡粉重量 (g)</label>
<input type="number" id="coffeeWeight" placeholder="输入咖啡粉重量" step="0.1">
<button class="btn btn-primary" onclick="setCoffeeWeight()" style="margin-top:12px;width:100%">设置</button>
</div>
<div class="input-group">
<label>目标水量 (g)</label>
<input type="number" id="targetWater" placeholder="输入目标水量" step="0.1">
<button class="btn btn-primary" onclick="setTargetWater()" style="margin-top:12px;width:100%">设置</button>
</div>
<div class="input-group">
<label>冲煮方法</label>
<select id="brewMethod">
<option value="Japanese">日式手冲</option>
<option value="American">美式手冲</option>
</select>
<button class="btn btn-primary" onclick="setBrewMethod()" style="margin-top:12px;width:100%">设置</button>
</div>
</div>
</div>
</div>
</div>
)rawliteral";

const char HTML_SCRIPT[] PROGMEM = R"rawliteral(
<script>
let updateInterval;
let lastWeight=0;
let lastTime='';
window.onload=function(){startUpdate();updateSegments()};
function startUpdate(){updateInterval=setInterval(updateData,50);updateData()}
function updateData(){
fetch('/api/weight').then(r=>r.json()).then(d=>{
const weightEl=document.getElementById('weight');
const newWeight=parseFloat(d.weight);
if(Math.abs(newWeight-lastWeight)>0.05){
weightEl.classList.add('updated');
setTimeout(()=>weightEl.classList.remove('updated'),300);
lastWeight=newWeight;
}
weightEl.textContent=newWeight.toFixed(1);
const ratioEl=document.getElementById('ratio');
if(d.coffeeWeight>0){
const r=newWeight/d.coffeeWeight;
ratioEl.textContent='粉水比: 1:'+r.toFixed(2);
ratioEl.style.display='inline-block';
}else{
ratioEl.textContent='';
ratioEl.style.display='none';
}
}).catch(e=>console.error('Error:',e));
fetch('/api/timer').then(r=>r.json()).then(d=>{
const timerEl=document.getElementById('timer');
timerEl.textContent=d.time;
if(d.state===1){
if(!timerEl.classList.contains('running')){
timerEl.classList.add('running');
}
}else{
timerEl.classList.remove('running');
}
if(d.segmentCount>0)updateSegments()
}).catch(e=>console.error('Error:',e))
}
function tare(){
fetch('/api/tare',{method:'POST'}).then(r=>r.json()).then(d=>{
if(d.success)showStatus('去皮成功','success')
}).catch(e=>showStatus('去皮失败','error'))
}
function startTimer(){
fetch('/api/timer/start',{method:'POST'}).then(r=>r.json()).then(d=>{
if(d.success)showStatus('计时器已开始','success')
})
}
function pauseTimer(){
fetch('/api/timer/pause',{method:'POST'}).then(r=>r.json()).then(d=>{
if(d.success)showStatus('计时器已暂停','success')
})
}
function stopTimer(){
fetch('/api/timer/stop',{method:'POST'}).then(r=>r.json()).then(d=>{
if(d.success)showStatus('计时器已停止','success')
})
}
function resetTimer(){
fetch('/api/timer/reset',{method:'POST'}).then(r=>r.json()).then(d=>{
if(d.success){
showStatus('计时器已重置','success');
updateSegments()
}
})
}
function addSegment(){
const n=document.getElementById('segmentName').value.trim();
if(!n){
showStatus('请输入分段名称','error');
return;
}
fetch('/api/timer/segment',{
method:'POST',
headers:{'Content-Type':'application/x-www-form-urlencoded'},
body:'name='+encodeURIComponent(n)
}).then(r=>r.json()).then(d=>{
if(d.success){
showStatus('分段已添加: '+n,'success');
document.getElementById('segmentName').value='';
setTimeout(updateSegments,100);
}
}).catch(e=>showStatus('添加分段失败','error'));
}
function updateSegments(){
fetch('/api/timer/segments').then(r=>r.json()).then(d=>{
const s=document.getElementById('segments');
s.innerHTML='';
if(d.count===0){
s.innerHTML='<div class="empty-state">暂无分段</div>';
return;
}
d.segments.forEach((seg,i)=>{
const item=document.createElement('div');
item.className='segment-item';
item.style.animationDelay=(i*0.1)+'s';
item.innerHTML='<div><span class="segment-name">'+(i+1)+'. '+seg.name+'</span></div><div class="segment-time">'+seg.time+'</div>';
s.appendChild(item);
});
}).catch(e=>console.error('Error:',e))
}
function setCoffeeWeight(){
const w=parseFloat(document.getElementById('coffeeWeight').value);
if(isNaN(w)||w<=0){
showStatus('请输入有效的重量','error');
return;
}
fetch('/api/setCoffeeWeight',{
method:'POST',
headers:{'Content-Type':'application/x-www-form-urlencoded'},
body:'weight='+w
}).then(r=>r.json()).then(d=>{
if(d.success)showStatus('咖啡粉重量已设置: '+w+'g','success')
}).catch(e=>showStatus('设置失败','error'));
}
function setTargetWater(){
const w=parseFloat(document.getElementById('targetWater').value);
if(isNaN(w)||w<=0){
showStatus('请输入有效的重量','error');
return;
}
fetch('/api/setTargetWater',{
method:'POST',
headers:{'Content-Type':'application/x-www-form-urlencoded'},
body:'weight='+w
}).then(r=>r.json()).then(d=>{
if(d.success)showStatus('目标水量已设置: '+w+'g','success')
}).catch(e=>showStatus('设置失败','error'));
}
function setBrewMethod(){
const m=document.getElementById('brewMethod').value;
fetch('/api/setBrewMethod',{
method:'POST',
headers:{'Content-Type':'application/x-www-form-urlencoded'},
body:'method='+encodeURIComponent(m)
}).then(r=>r.json()).then(d=>{
if(d.success)showStatus('冲煮方法已设置: '+m,'success')
}).catch(e=>showStatus('设置失败','error'));
}
function showStatus(msg,type){
const s=document.createElement('div');
s.className='status '+type;
s.textContent=msg;
s.style.position='fixed';
s.style.top='20px';
s.style.left='50%';
s.style.transform='translateX(-50%)';
s.style.zIndex=1000;
s.style.minWidth='250px';
s.style.maxWidth='90%';
document.body.appendChild(s);
setTimeout(function(){s.style.opacity='0';s.style.transform='translateX(-50%) translateY(-20px)';setTimeout(()=>s.remove(),300)},3000);
}
</script>
</body>
</html>
)rawliteral";

#endif
