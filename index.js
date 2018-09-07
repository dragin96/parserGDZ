var rp = require("request-promise");
var cheerio = require("cheerio");

rp("https://gdz.ru/class-1/matematika/moro-m-i-2011/")
	.then(function (html) {
		console.log("получили разметку");
		var $ = cheerio.load(html);
		const taskList = $(".task-list");
		console.log(taskList.children().eq(0).children().is("section"));
		console.log(taskList.children().eq(0).children().is("section"));
		getURLs(taskList.children());

	})
	.catch(function (err) {
		console.log("возникла ошибка: ", err);
	});


/*
	передать элементы, 
	возвратит обьект ссылками на ответ пример:
	{
		'чаcть1': [{url, num}]
		'чаcть2': 
			подчасть1: [{url, num}],
			подчасть2: [{url, num}]
	}
*/
function getURLs(el){
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
		else{
			console.log("context", context);
			console.log("setion not");
			fillRez(el,context);
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
			const url = task.attr("href");
			const num = task.attr("title");
			//console.log("url", url);
			//console.log("num", num);
			tmpArr.push({ url, num });
		}
		if (context){
			obj[context][header] = tmpArr;
		}
		else{
			obj[header] = tmpArr;
		}
	}
	console.debug(obj);
	return obj;
}
