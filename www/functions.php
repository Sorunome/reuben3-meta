<?php
function getBinDiget($s){
	$s = strtolower($s);
	switch($s){
		case '0':
			return '0000';
		case '1':
			return '0001';
		case '2':
			return '0010';
		case '3':
			return '0011';
		case '4':
			return '0100';
		case '5':
			return '0101';
		case '6':
			return '0110';
		case '7':
			return '0111';
		case '8':
			return '1000';
		case '9':
			return '1001';
		case 'a':
			return '1010';
		case 'b':
			return '1011';
		case 'c':
			return '1100';
		case 'd':
			return '1101';
		case 'e':
			return '1110';
		case 'f':
			return '1111';
	}
}
function hexstr2binstr($s){
	$out = '';
	for($i = 0;$i<strlen($s);$i++){
		$out .= getBinDiget($s[$i]);
	}
	return $out;
}
function getBigSpriteEditor($url,$data,$name,$enemy = false){
	return '<style type="text/css">
			#container {
				padding:0;
				margin:0;
				line-height:0;
			}
			.pixel {
				display:inline-block;
				width:20px;
				height:20px;
				padding:0;
				margin:0;
				border:1px solid #3D2F2F;
			}
			.pixel.color0 {
				background-color:#FFFFFF;
			}
			.pixel.color1 {
				background-color:#C0C0C0;
			}
			.pixel.color2 {
				background-color:#606060;
			}
			.pixel.color3 {
				background-color:#000000;
			}
		</style>
		<script type="text/javascript">
			function pad(n, width, z){
				z = z || "0";
				n = n + "";
				return n.length >= width ? n : new Array(width - n.length + 1).join(z) + n;
			}
			$(document).ready(function(){
				var enemyData = '.$data.',
					spriteData = [],
					name = '.json_encode(['name' => $name]).',
					populateBuffers = function(){
						var frontBufHex = pad(enemyData.frontBuf,enemyData.height*enemyData.width*2),
							backBufHex = pad(enemyData.backBuf,enemyData.height*enemyData.width*2);
						spriteData = [];
						for(var j = 0;j < enemyData.height;j++){
							spriteData.push([]);
							for(var i = 0;i < (enemyData.width*2);i++){
								var frontBufBin = pad(parseInt(frontBufHex[j*enemyData.width*2 + i],16).toString(2),4),
									backBufBin = pad(parseInt(backBufHex[j*enemyData.width*2 + i],16).toString(2),4);
								for(var l = 0;l < 4;l++){
									var k = 0;
									if(frontBufBin[l]=="1"){
										k += 2;
									}
									if(backBufBin[l]=="1"){
										k += 1;
									}
									spriteData[j].push(k);
								}
							}
						}
					},
					drawBoard = function(){
						$("#container").empty().append(
							$.map(spriteData,function(row,i){
								return $.map(row,function(p,j){
									return $("<div>")
										.addClass("pixel")
										.addClass("color"+p.toString(10))
										.data({
											i:i,
											j:j
										})
										.mousedown(function(e){
											i = $(this).data("i");
											j = $(this).data("j");
											$(this).removeClass (function (index, css) {
												return (css.match (/(^|\s)color\S+/g) || []).join(" ");
											});
											if(e.which == 1){ // left click
												e.preventDefault();
												
												if(spriteData[i][j] < 2){
													spriteData[i][j]+=2;
												}else{
													spriteData[i][j]-=2;
												}
											}else if(e.which == 3){ // right click
												e.preventDefault();
												if(spriteData[i][j] == 3 || spriteData[i][j] == 1){
													spriteData[i][j] -= 1;
												}else{
													spriteData[i][j] += 1;
												}
											}
											$(this).addClass("color"+(spriteData[i][j]).toString(10));
										});
								});
							})
						).width(enemyData.width*8*22);
					};
				populateBuffers();
				drawBoard();
				$("#otherCont").append(
					$("<a>").text("Rotate Up").click(function(e){
						e.preventDefault();
						var t = spriteData.shift();
						spriteData.push(t);
						drawBoard();
					}),"<br>",
					$("<a>").text("Rotate Down").click(function(e){
						e.preventDefault();
						var t = spriteData.pop();
						spriteData.unshift(t);
						drawBoard();
					}),"<br>",
					$("<a>").text("Rotate Left").click(function(e){
						e.preventDefault();
						for(var i = 0;i < enemyData.height;i++){
							var t = spriteData[i].shift();
							spriteData[i].push(t);;
						}
						drawBoard();
					}),"<br>",
					$("<a>").text("Rotate right").click(function(e){
						e.preventDefault();
						for(var i = 0;i < enemyData.height;i++){
							var t = spriteData[i].pop();
							spriteData[i].unshift(t);;
						}
						drawBoard();
					}),"<br>",
					"Width:",$("<input>").attr("type","number").val(enemyData.width).change(function(e){
							var newWidth = parseInt(this.value,10);
							for(var j = 0;j < spriteData.length;j++){
								if(newWidth < enemyData.width){
									spriteData[j].length = newWidth*8;
								}else{
									for(var i = 0;i < (newWidth - enemyData.width)*8;i++){
										spriteData[j].push(0);
									}
								}
							}
							enemyData.width = newWidth;
							drawBoard();
						}),"<br>",
					"Height:",$("<input>").attr("type","number").val(enemyData.height).change(function(e){
							var newHeight = parseInt(this.value,10);
							if(newHeight < enemyData.height){
								spriteData.length = newHeight;
							}else{
								var a = [];
								for(var j = 0;j < enemyData.width*8;j++){
									a.push(0);
								}
								for(var i = 0;i < (newHeight - enemyData.height);i++){
									spriteData.push(a);
								}
							}
							enemyData.height = newHeight;
							drawBoard();
						}),"<br>",
					"Name:",$("<input>").attr("type","text").val(name.name).change(function(){name.name = this.value}),"<br>"
					'.($enemy?'
					,"Level:",$("<input>").attr("type","number").val(enemyData.level).change(function(){enemyData.level = parseInt(this.value,10);}),"<br>",
					"Hitpoints:",$("<input>").attr("type","number").val(enemyData.hp).change(function(){enemyData.hp = parseInt(this.value,10);}),"<br>",
					"EXP:",$("<input>").attr("type","number").val(enemyData.exp).change(function(){enemyData.exp = parseInt(this.value,10);}),"<br>",
					"X-Pos:",$("<input>").attr("type","number").val(enemyData.xpos).change(function(){enemyData.xpos = parseInt(this.value,10);}),"<br>",
					"Y-Pos:",$("<input>").attr("type","number").val(enemyData.ypos).change(function(){enemyData.ypos = parseInt(this.value,10);}),"<br>",
					"Wait:",$("<input>").attr("type","number").val(enemyData.wait).change(function(){enemyData.wait = parseInt(this.value,10);}),"<br>",
					"Fire defence:",$("<input>").attr("type","number").val(enemyData.firedef).change(function(){enemyData.firedef = parseInt(this.value,10);}),"<br>",
					"Ice defence:",$("<input>").attr("type","number").val(enemyData.icedef).change(function(){enemyData.icedef = parseInt(this.value,10);}),"<br>",
					"Bolt defence:",$("<input>").attr("type","number").val(enemyData.boltdef).change(function(){enemyData.boltdef = parseInt(this.value,10);}),"<br>",
					"Bomb defence:",$("<input>").attr("type","number").val(enemyData.bombdef).change(function(){enemyData.bombdef = parseInt(this.value,10);}),"<br>",
					"Boss:",$("<input>").attr("type","checkbox").attr((enemyData.boss?"checked":"false"),"checked").change(function(){enemyData.boss = this.checked;}),"<br>",
					$.map([1,2,3,4,5],function(i){
						return [
							"Attack slot ",i,":",
							$("<select>").append(
								$.map(["Normal","Fire","Bolt","Poison","Stun"],function(n,ii){
									return $("<option>").val(ii).text(n).attr((ii == enemyData["sl"+i]?"selected":"false"),"selected");
								})
							).change(function(){enemyData["sl"+i] = parseInt(this.value,10);}),"<br>"
						]
					})
					':'').'
				);
				$("#save").click(function(e){
					e.preventDefault();
					var frontBufferBinary = "",
						frontBufferHex = "",
						backBufferBinary = "",
						backBufferHex = "";
					for(var i = 0;i < enemyData.height;i++){
						for(j = 0;j < enemyData.width*8;j++){
							if(spriteData[i][j] >= 2){
								frontBufferBinary += "1";
							}else{
								frontBufferBinary += "0";
							}
							if(spriteData[i][j]%2 == 1){
								backBufferBinary += "1";
							}else{
								backBufferBinary += "0";
							}
							if(frontBufferBinary.length % 8 == 0){
								frontBufferHex += pad(parseInt(frontBufferBinary,2).toString(16),frontBufferBinary.length / 4);
								backBufferHex += pad(parseInt(backBufferBinary,2).toString(16),backBufferBinary.length / 4);
								frontBufferBinary = "";
								backBufferBinary = "";
							}
						}
					}
					enemyData.frontBuf = frontBufferHex;
					enemyData.backBuf = backBufferHex;
					homepage.post("'.$url.'?save='.(int)$_GET['edit'].'",{
						"json":JSON.stringify(enemyData),
						"name":name.name
					},function(data){
						alert(data);
					});
				});
				$("#delete").click(function(e){
					e.preventDefault();
					if(confirm("are you sure you want to delete this '.$url.'?")){
						homepage.get("'.$url.'?delete='.(int)$_GET['edit'].'",function(data){
							getPageJSON("'.$url.'");
						});
					}
				});
			});
		</script>
		<div id="container" oncontextmenu="return false;"></div><div id="otherCont"></div>
		<button id="save">Save</button><button id="delete" style="float:right;">Delete</button>
		<br><br>
		<a href="'.$url.'">&lt;&lt; Back</a>';
}
?>
