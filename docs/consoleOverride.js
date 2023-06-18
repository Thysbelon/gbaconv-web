const newlog=document.getElementById("debug")
function CustomLogNormal(msg) {
  newlog.innerHTML+=msg+"\n"
}
function CustomLogError(msg) {
  newlog.innerHTML+="(❗)"+msg+"\n"
}
function CustomLogWarn(msg) {
  newlog.innerHTML+="⚠️"+msg+"\n"
}
window.console.log = CustomLogNormal;
window.console.warn = CustomLogWarn;
window.console.error = CustomLogError;
window.console.assert = CustomLogError;