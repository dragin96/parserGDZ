/*
    передать элементы,
    возвратит обьект ссылками на ответ пример:
    {
        'чаcть1': [{url, num}]
		'чаcть2':[],
		'часть2/подчасть'
    }
*/
function getURLs(el, urlBook) {
	let obj = {};
	getStruct(el);
	function getStruct(el, context = "") {
		if (el.is("section")) {
			let sections = el;
			for (let index = 0; index < sections.length; index++) {
				const section = sections.eq(index);
				let contextCreat = section.eq(0).children().parent("section").parent("section").children("header").text().replace(/\s\W\s+/g, "");
				getStruct(section.eq(0).children(), contextCreat);
			}
		}
		else {
			console.log("context", context);
			console.log("setion not");
			fillRez(el, context);
		}
	}
	function fillRez(el, context) {
		//el = el.children()
		let header = el.eq(0).text().replace(/\s\W\s+/g, '');
		console.log("get header", header);
		let tmpArr = [];
		let listTask = el.eq(1).find("a");
		for (let index = 0; index < listTask.length; index++) {
			const task = listTask.eq(index);
			const tmpUrl = task.attr("href");
			const url = /#task\?t=/.test(tmpUrl) ? urlBook + tmpUrl.replace(/#task\?t=/, "") : "https://gdz.ru" + tmpUrl;
			const num = task.attr("title");
			console.log("urlIMG", url);
			//console.log("num", num);
			tmpArr.push({ url, num });
		}
		context ? obj[context+"/"+header] = tmpArr : obj[header] = tmpArr;
	}
	console.debug(obj);
	return obj;
}
exports.getURLs = getURLs;