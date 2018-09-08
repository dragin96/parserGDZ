const rp = require("request-promise");

function getImg(url) {
	var options = {
		headers: {
			"accept": "application/json, text/javascript, */*; q=0.01",
			"authority": "gdz.ru",
			"x-requested-with": "XMLHttpRequest",
		},
		json: true // Automatically parses the JSON string in the response
	};
	options.uri = url;
	rp(options)
		.then(function (html) {
			let rez = [];
			console.log("получили json таска");
			let listSolve = html.editions;
			listSolve.forEach(solve => {
				let imgs = solve.images;
				for (let num in imgs) {
					rez.push("https://gdz.ru" + imgs[num].url);
				}
			});
			console.log(rez);
			return rez;
		})
		.catch(function (err) {
			console.log("возникла ошибка: ", err);
		});
}
exports.getImg = getImg;