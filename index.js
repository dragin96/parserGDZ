//const { getTask } = require("./getTask");

const {
	getURLs
} = require("./getURLs");
const {
	getImg
} = require("./getImg");

const rp = require("request-promise");
const cheerio = require("cheerio");
const urlDownload = "https://gdz.ru/class-1/chelovek-i-mir/trafimova/";

let rez = {
	book_name: getNameBook(urlDownload),
	tasks: {}
};
var express = require('express');
var app = express();

var bodyParser = require('body-parser');
app.use(bodyParser.json());

app.listen(1080, function() {
});

app.post("/getTasks", function(req, res) {
    var data = req.body;
	
	rp(urlDownload)
	.then(function (html) {
		console.log("получили разметку учебника");
		const $ = cheerio.load(html);
		const taskList = $(".task-list");
		console.log(taskList.children().eq(0).children().is("section"));
		console.log(taskList.children().eq(0).children().is("section"));
		let urls = getURLs(taskList.children(), urlDownload);
		//console.log(urls);
		getTask(urls);
	})
	.catch(function (err) {
		console.log("возникла ошибка: ", err);
	});


 async function getTask(urls) {
	let tmpArr = [];
	//console.log(urls);
	for (const key in urls) {
		console.log("создать папку: " + key);
		rez["tasks"][key] = []
		const razdelTask = urls[key];
		let ii = 0;
		let countError = 0;
		for(let element of razdelTask){
			console.log(++ii, razdelTask.length);
			await getImg(element.url).then((el)=>{
				rez["tasks"][key][ii] = el;
				countError = 0;
				console.log(el);
			}).catch(async ()=>{
				if(countError<5){
					rez["tasks"][key][ii] = await getImg(element.url);
				}
				console.log('произошла ошибка с', ii);
			});
		}
		
		/*
		razdelTask.forEach(async element => {
			console.log(++ii, razdelTask.length);
			rez["tasks"][key] = await getImg(element.url).catch(()=>{
				console.log('произошла ошибка с', ii);
			});
		});*/
	}
	res.send(rez);

}
});


function getNameBook(url) {
	return url.match(/\/[A-Za-z0-9\-\.]+\/$/g)[0];
}
