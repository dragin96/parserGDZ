//const { getTask } = require("./getTask");

const {
	getURLs
} = require("./getURLs");
const {
	getImg
} = require("./getImg");

const rp = require("request-promise");
const cheerio = require("cheerio");
let urlDownload = "https://gdz.ru/class-1/chelovek-i-mir/trafimova/";

let rez = {
	book_name: "",
	tasks: {}
};
var express = require('express');
var app = express();

var bodyParser = require('body-parser');
app.use(bodyParser.json());

app.listen(1080, function() {
});

let isProcessing=false;
app.post("/getTasks", function(req, res) {
	res.setTimeout(30*60*1000);
	var data = req.body;
	console.log('data', req.body);
	if(!data.url){
		return res.send("error");
	}
	if(isProcessing){
		return res.send("Запрос уже обрабатывается");
	}
	urlDownload = data.url;
	rez.book_name=getNameBook(urlDownload);
	rp(urlDownload)
	.then(function (html) {
		console.log("получили разметку учебника");
		const $ = cheerio.load(html);
		const taskList = $(".task-list");
		//console.log(taskList.children().eq(0).children().is("section"));
		let urls = getURLs(taskList.children(), urlDownload);
		//console.log(urls);
		isProcessing=true;
		getTask(urls);
	})
	.catch(function (err) {
		console.log("возникла ошибка: ", err);
	});


 async function getTask(urls) {
	let tmpArr = [];
	//console.log(urls);
	for (const key in urls) {
		//console.log("создать папку: " + key);
		rez["tasks"][key] = {};
		const razdelTask = urls[key];
		let ii = 0;
		let countError = 0;
		for(let element of razdelTask){
			console.log(++ii, razdelTask.length);
			element.num = element.num.replace('§', '');
			rez["tasks"][key][element.num] = {};
			const el = await getImg(element.url).catch(async ()=>{
				console.error("Произошла ошибка с ", ii, element.num, key);
				for(let i=0; i < 5; i++){
					const el = await getImg(element.url).catch(()=>{
						console.error("Это уже ", i, "попытка");
					});
					if(el) {
						rez["tasks"][key][element.num] = el;
						break;
					}
				}
			});
			rez["tasks"][key][element.num] = el;
			
			/*await getImg(element.url).then((el)=>{
				rez["tasks"][key][element.num] = el;
				countError = 0;
				console.log(el);
			}).catch(async ()=>{
				if(countError<5){
					rez["tasks"][key][element.num] = await getImg(element.url);
				}
				console.log('произошла ошибка с', ii);
			});*/
		}
		
		/*
		razdelTask.forEach(async element => {
			console.log(++ii, razdelTask.length);
			rez["tasks"][key] = await getImg(element.url).catch(()=>{
				console.log('произошла ошибка с', ii);
			});
		});*/
	}
	isProcessing=false;
	res.send(rez);
	rez = {
		book_name: "",
		tasks: {}
	};
}
});


function getNameBook(url) {

	return url.match(/\/[A-Za-z0-9\-\.]+\/$/g)[0];
}
