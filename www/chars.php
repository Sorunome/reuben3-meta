<?php
if(!$security->isLoggedIn() || !($user_info['power']&32)){
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("/");</script>Redirecting...',$lang,$pathPartsParsed);
	die();
}
$sql->switchDb('soru_reuben3_meta');
include_once(realpath(dirname(__FILE__)).'/functions.php');
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(E_ALL);
$html = '';

if(isset($_GET['edit'])){
	$char = $sql->query("SELECT `bighex`,`smallhex`,`char` FROM `font` WHERE `id`=%d",[(int)$_GET['edit']],0);
	$html .= '<h2>'.$char['char'].'</h2>
		<style type="text/css">
			#bigcontainer,#smallcontainer {
				padding:0;
				margin:0;
				line-height:0;
			}
			#bigcontainer {
				width:210px;
			}
			#smallcontainer {
				width:126px;
			}
			.pixel {
				display:inline-block;
				width:40px;
				height:40px;
				padding:0;
				margin:0;
				border:1px solid #3D2F2F;
				background-color:#FFFFFF;
			}
			.pixel.on {
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
				var bighex = pad("'.$char['bighex'].'",7*2),
					bigData = [],
					smallhex = pad("'.$char['smallhex'].'",5*2),
					smallData = [],i,j,k,
					getBoard = function(big){
						return $.map(big?bigData:smallData,function(row,i){
							return $.map(row,function(p,j){
								return $("<div>")
									.addClass("pixel")
									.addClass(p?"on":"")
									.data({
										i:i,
										j:j
									})
									.click(function(e){
										var i = $(this).data("i"),
											j = $(this).data("j");
										$(this).toggleClass("on");
										if(big){
											bigData[i][j] = !bigData[i][j];
										}else{
											smallData[i][j] = !smallData[i][j];
										}
									})
							});
						});
					};
				
				for(i = 0;i < 7;i++){
					bigData.push([]);
					var bin = pad(parseInt(bighex.substr(2*i,2),16).toString(2),8);
					for(j = 0;j < 5;j++){
						bigData[i].push(bin[j]=="1"?true:false);
					}
				}
				for(i = 0;i < 5;i++){
					smallData.push([]);
					var bin = pad(parseInt(smallhex.substr(2*i,2),16).toString(2),8);
					for(j = 0;j < 3;j++){
						smallData[i].push(bin[j]=="1"?true:false);
					}
				}
				
				$("#bigcontainer").append(getBoard(true));
				$("#smallcontainer").append(getBoard(false));
				
				$("#save").click(function(e){
					e.preventDefault();
					var bigDataBinary = "",
						bigDataHex = "",
						smallDataBinary = "",
						smallDataHex = "";
					for(i = 0;i < 7;i++){
						for(j = 0;j < 5;j++){
							bigDataBinary += (bigData[i][j]?"1":"0");
						}
						bigDataBinary += "000";
						bigDataHex += pad(parseInt(bigDataBinary,2).toString(16),2);
						bigDataBinary = "";
					}
					for(i = 0;i < 5;i++){
						for(j = 0;j < 3;j++){
							smallDataBinary += (smallData[i][j]?"1":"0");
						}
						smallDataBinary += "00000";
						smallDataHex += pad(parseInt(smallDataBinary,2).toString(16),2);
						smallDataBinary = "";
					}
					homepage.post("chars?save='.(int)$_GET['edit'].'",{
						"bigData":bigDataHex,
						"smallData":smallDataHex
					},function(data){
						alert(data);
					});
				});
				
				$("#delete").click(function(e){
					e.preventDefault();
					if(confirm("are you sure you want to delete this char?")){
						homepage.get("chars?delete='.(int)$_GET['edit'].'",function(data){
							getPageJSON("chars");
						});
					}
				});
			});
		</script>
		<div id="bigcontainer" oncontextmenu="return false;"></div>
		<br>
		<div id="smallcontainer" oncontextmenu="return false;"></div>
		<br>
		<button id="save">Save</button><button id="delete" style="float:right;">Delete</button>
		<br><br>
		<a href="chars">&lt;&lt; Back</a>';
}elseif(isset($_GET['new'])){
	$sql->query("INSERT INTO `font` (`bighex`,`smallhex`,`char`) VALUES ('00000000000000','0000000000','%s')",[$_GET['new']]);
	header('Content-type: text/json');
	echo json_encode([
		'id' => $sql->insertId()
	]);
}elseif(isset($_GET['save'])){
	$sql->query("UPDATE `font` SET `bighex`='%s',`smallhex`='%s' WHERE `id`=%d",[$_POST['bigData'],$_POST['smallData'],(int)$_GET['save']]);
	echo 'Saved!';
}elseif(isset($_GET['delete'])){
	$sql->query("DELETE FROM `font` WHERE `id`=%d",[(int)$_GET['delete']]);
}else{
	$font = $sql->query("SELECT `char`,`id` FROM `font`");
	foreach($font as $f){
		$html .= '<a href="chars?edit='.$f['id'].'">'.str_replace(' ','&nbsp;',$f['char']).'</a> ';
	}
	$html .= '<br>
	<button id="new">New</button>
	<script type="text/javascript">
		$("#new").click(function(e){
			e.preventDefault();
			var char = prompt("char");
			if(char!="" && char!=undefined && char!=null && char.length==1){
				homepage.get("chars?new="+char,function(data){
					getPageJSON("chars?edit="+data.id);
				})
			}
		});
	</script>
	<br>
	<a href=".">&lt;&lt; Back</a>';
}

if($html != ''){
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Chars',$html,$lang,$pathPartsParsed);
} // 0 starts at 0x01
?>
