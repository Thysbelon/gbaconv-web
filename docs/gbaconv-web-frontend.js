const savpicker=document.getElementById("savpicker")
savpicker.addEventListener('change', myClick);
function myClick(e){
	// replace 4gs with srm
	// replace whatever the file name is with "savedata", c can't handle paths with spaces !!!!
	var filename=e.target.files[0].name.replace(/.*\.(.*)/, "home/web_user/savedata.$1").replace(".4gs", ".srm").replace(".4gz", ".srm").replace(".44gbasav",".srm")
	console.log(filename)
	const reader=new FileReader();
	reader.readAsArrayBuffer(e.target.files[0]);
	reader.onloadend = (evt) => {
		if (evt.target.readyState === FileReader.DONE) {
			let arrayBuffer = evt.target.result
			var data = new Uint8Array(arrayBuffer);
			if (e.target.files[0].name.includes(".4gz")) {
				data = pako.ungzip(toyEncrypt(data))
			}
			for (let i=0; i<10; i++) {
				console.log(data[i])
			}
			//FS.mount(IDBFS, {}, '/data');
			FS.writeFile("/"+filename, data, {flags:"w+"})
			Module.ccall(
				"GBAConv",
				"number",
				["string"],
				["/"+filename]
			)
			if (filename.includes(".srm")){filename=filename.replace(".srm", ".sav")} else {filename=filename.replace(".sav", ".srm")}
			//const request = indexedDB.open("/data");
			//request.onsuccess = (event) => {
			//	db = event.target.result;
			//	db.transaction("FILE_DATA").objectStore("FILE_DATA").get( "/"+filename ).onsuccess = (event) => {
			//		var savedataObj=event.target.result.contents
			//		savedataValues=Object.values(savedataObj) // array
			//		//const buffer=new ArrayBuffer(romdataValues.length)
			//		//const view = new Uint8Array(buffer);
			//		const savedata8array = Uint8Array.from(savedataValues);
			//		
			//		const file = new File([savedata8array], e.target.files[0].name.replace(/\.\w\w\w/, "."+filename.slice(-3) ) )
			//		download(file)
			//		//var req = indexedDB.deleteDatabase("/data");
			//		//req.onsuccess = function () {
			//		//	console.log("Deleted database successfully");
			//		//};
			//		//FS.unlink("/data/savedata.sav")
			//		//FS.unlink("/data/savedata.srm")
			//		//FS.unmount('/data');
			//		//FS.rmdir("/data")
			//		if (typeof DebugOn === 'undefined') {
			//			window.location.reload(); // I tried so hard to make this app work without reloading, but all of my attempts to delete the contents of memfs failed.
			//		}
			//	};
			//};
			console.log("filename: "+filename)
			//stream = FS.open(filename, "r")
			//FS.close(stream)
			savedata8array=FS.readFile('/'+filename) // is opts optional?
			const file = new File([savedata8array], e.target.files[0].name.replace(/\.\w*$/, "."+filename.slice(-3) ) )
			download(file)
			FS.unlink('/'+filename);
			//if (typeof DebugOn === 'undefined') {
			//	window.location.reload(); // I tried so hard to make this app work without reloading, but all of my attempts to delete the contents of memfs failed.
			//}
			
		}
	}
	
}

function download(file) {
	const link = document.createElement('a')
	const url = URL.createObjectURL(file)
	
	link.href = url
	link.download = file.name
	document.body.appendChild(link)
	link.click()
	
	document.body.removeChild(link)
	window.URL.revokeObjectURL(url)
}

// copy pasted from https://gba.44670.org/app.js
function toyEncrypt(src) {
	var dst = new Uint8Array(src.length)
	for (var i = 0; i < src.length; i++) {
		dst[i] = src[i] ^ 0xFB
	}
	return dst
}