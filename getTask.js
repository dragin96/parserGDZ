function getTask(urls) {
	let tmpArr = [];
	for (const key in urls) {
		//console.log("создать папку: " + key);
		const razdelTask = urls[key];
		razdelTask.forEach(element => {
			//console.log(element.url);
		});
	}
}
exports.getTask = getTask;