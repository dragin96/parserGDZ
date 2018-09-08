//const { getTask } = require("./getTask");

const { getURLs } = require("./getURLs");
const { getImg } = require("./getImg");

const rp = require("request-promise");
const cheerio = require("cheerio");
const urlDownload = "https://gdz.ru/class-6/matematika/didakticheskie-materiali-merzlyak/";

let rez = {
	book_name: getNameBook(urlDownload),
	tasks: {}
};

function getNameBook(url) {
	return url.match(/\/[A-Za-z0-9\-\.]+\/$/g)[0];
}
rp(urlDownload)
	.then(function (html) {
		console.log("получили разметку учебника");
		const $ = cheerio.load(html);
		const taskList = $(".task-list");
		console.log(taskList.children().eq(0).children().is("section"));
		console.log(taskList.children().eq(0).children().is("section"));
		let urls = getURLs(taskList.children(),urlDownload);
		//console.log(urls);
		getTask(urls);
	})
	.catch(function (err) {
		console.log("возникла ошибка: ", err);
	});


function getTask(urls) {
	let tmpArr = [];
	//console.log(urls);
	for (const key in urls) {
		console.log("создать папку: " + key);
		rez["tasks"][key] = []
		const razdelTask = urls[key];
		razdelTask.forEach(element => {
			rez["tasks"][key] = getImg(element.url);
		});
	}
	console.log("this is rez", rez);
	
}