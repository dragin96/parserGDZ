const rp = require("request-promise");

function getImg(url) {
	return new Promise(async (resolve, reject) => {
		var options = {
			headers: {
				"accept": "application/json, text/javascript, */*; q=0.01",
				"authority": "gdz.ru",
				"x-requested-with": "XMLHttpRequest",
			},
			json: true // Automatically parses the JSON string in the response
		};
		options.uri = url;
		await rp(options)
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
				resolve(rez);
			})
			.catch(function (err) {
				console.log("возникла ошибка: ", err);
				reject();
			});
	})
}
exports.getImg = getImg;