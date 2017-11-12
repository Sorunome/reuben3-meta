<?php
ini_set('display_errors',1);
ini_set('display_startup_errors',1);
error_reporting(E_ALL);
if(!$security->isLoggedIn() || !($user_info['power']&32)){
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("/");</script>Redirecting...',$lang,$pathPartsParsed);
	die();
}

$SPRITESROOT = '/var/www/www.sorunome.de/reuben3-meta/sprites/';

$sql->switchDb('soru_reuben3_meta');
include_once(realpath(dirname(__FILE__)).'/functions.php');
if(isset($_GET['edit'])){
	$id = (int)$_GET['edit'];
	$html = '<style type="text/css">
				#container {
					width:336px;
					padding:0;
					margin:0;
					line-height:0;
				}
				.pixel {
					display:inline-block;
					width:40px;
					height:40px;
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
					var spriteData = [';
	
						$r = $sql->query("SELECT `buffer1`,`buffer2` FROM `sprites` WHERE `id`=%d",[$id],0);
						$buffer1bin = hexstr2binstr($r['buffer1']);
						$buffer2bin = hexstr2binstr($r['buffer2']);
						$s = '';
						$k = 0;
						for($i = 0;$i < 8;$i++){
							$s .= '[';
							for($j = 0;$j < 8;$j++){
								$c = $buffer1bin[$k].$buffer2bin[$k];
								switch($c){
									case '00':
										$s .= '0,';
										break;
									case '01':
										$s .= '1,';
										break;
									case '10':
										$s .= '2,';
										break;
									case '11':
										$s .= '3,';
										break;
								}
								$k++;
							}
							$s = substr($s,0,-1).'],';
						}
						$html .= substr($s,0,-1);
					$html .= '],
						i,j;
					for(i=0;i<8;i++){
						for(j=0;j<8;j++){
							$("#container").append(
								$("<div>")
									.addClass("pixel")
									.addClass("color"+(spriteData[i][j]).toString(10))
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
									})
							);
						}
					}
					$("#save").click(function(e){
						e.preventDefault();
						var buffer1Binary = "",
							buffer1Hex = "",
							buffer2Binary = "",
							buffer2Hex = "";
						for(i=0;i<8;i++){
							for(j=0;j<8;j++){
								if(spriteData[i][j] >= 2){
									buffer1Binary += "1";
								}else{
									buffer1Binary += "0";
								}
								if(spriteData[i][j]%2 == 1){
									buffer2Binary += "1";
								}else{
									buffer2Binary += "0";
								}
							}
							buffer1Hex += pad(parseInt(buffer1Binary,2).toString(16),2);
							buffer2Hex += pad(parseInt(buffer2Binary,2).toString(16),2);
							buffer1Binary = "";
							buffer2Binary = "";
						}
						
						homepage.post("sprites?save=';
							$html .= $_GET['edit'];
							$html .= '",
						{
							"buffer1":buffer1Hex,
							"buffer2":buffer2Hex
						},
						function(data){
							alert(data);
						});
					});
					$("#delete").click(function(e){
						e.preventDefault();
						if(confirm("are you sure you want to delete this sprite?")){
							homepage.get("sprites?delete='.(int)$_GET['edit'].'",function(data){
								getPageJSON("sprites");
							});
						}
					});
					$("#upload").on("submit", function(e) {
						e.preventDefault();
						$.ajax({
							url: "sprites?upload='.(int)$_GET['edit'].'",
							type: "POST",
							data: new FormData(this),
							contentType: false,
							cache: false,
							processData: false,
							success: function(data) {
								alert(data);
							}
						});
					});
				});
			</script>
			<h1>Sprite ID: '.(int)$_GET['edit'].'</h1>
			<div id="container" oncontextmenu="return false;">
			</div>';
	if (file_exists($SPRITESROOT.(string)$id.'.png')) {
		$html .= '<img src="disp?sprite='.(string)$id.'&amp;scale=12" alt="sprite">';
	}
	$html .= '<form id="upload" action="" method="post" encode="multipart/form-data">
				<input type="file" name="file" required>
				<input type="submit" value="Upload">
			</form>
			<button id="save">Save</button><button id="delete" style="float:right;">Delete</button><br>
			<a href="sprites">&lt;&lt; Back</a>';
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Edit Sprite',$html,$lang,$pathPartsParsed);
}elseif(isset($_GET['save'])){
	$sql->query("UPDATE `sprites` SET `buffer1`='%s',`buffer2`='%s' WHERE `id`=%d",[$_POST['buffer1'],$_POST['buffer2'],(int)$_GET['save']]);
	echo 'Saved';
}elseif(isset($_GET['delete'])){
	$sql->query("DELETE FROM `sprites` WHERE `id`=%d",[(int)$_GET['delete']]);
} elseif (isset($_GET['upload'])) {
	$id = (int)$_GET['upload'];
	if ($id != $_GET['upload']) {
		die('non-int ID');
	}
	$i = $sql->query("SELECT `id` FROM `sprites` WHERE `id`=%d",[$id],0);
	if ($i['id'] === NULL) {
		die('invalid ID');
	}
	is_uploaded_file($_FILES['file']['tmp_name']) or die('no file to upload');
	$fname = $SPRITESROOT.(string)$id.'.png';
	if (file_exists($fname)) {
		unlink($fname);
	}
	move_uploaded_file($_FILES['file']['tmp_name'], $fname) or die('could not move tmp file');
	if (!($j = @imagecreatefromstring(file_get_contents($fname)))) {
		unlink($fname);
		die('invalid image');
	}
	if (imagesx($j) != 8 || imagesy($j) != 8) {
		unlink($fname);
		die('image needs to be 8x8');
	}
	unlink($fname);
	ob_start();
	imagepng($j);
	$b = ob_get_contents();
	ob_end_clean();
	file_put_contents($fname, $b);
	imagedestroy($j);
	
	die('Successfully uploaded image');
}elseif(isset($_GET['new'])){
	$sql->query("INSERT INTO `sprites` (`buffer1`,`buffer2`) VALUES ('0000000000000000','0000000000000000')");
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("sprites");</script>Redirecting...',$lang,$pathPartsParsed);
}elseif(isset($_GET['info'])){
	header('Content-Type: text/json');
	$i = $sql->query("SELECT `id`,`name` FROM `sprites` WHERE `id`=%d",[(int)$_GET['info']],0);
	$info = [];
	$info['id'] = (int)$i['id'];
	if($i['name']){
		$info['name'] = $i['name'];
	}
	echo json_encode($info);
}else{
	$html = '<style type="text/css">
		#spritesCont > a {
			margin:0;
			padding:0;
		}
		#spritesCont > a > .img {
			display:inline-block;
			width:32px;
			height:32px;
			background-image:url("disp?spritesheet&amp;scale=4&amp;'.time().'");
			border:1px solid red;
			margin:0;
			padding:0;
		}
		</style>
		<div id="spritesCont">';
	
	$sprites = $sql->query("SELECT `id` FROM `sprites` WHERE 1");
	foreach($sprites as $s){
		$html .= '<a href="sprites?edit='.$s['id'].'"><div class="img" style="background-position:-'.(8*4*($s['id'] % 15)).'px -'.(8*4*floor($s['id'] / 15)).'px"></div></a>';
	}
	$html .='</div><hr>
	<a href="sprites?new">New</a><br>
	<a href="/reuben3-meta">&lt;&lt; Back</a>';
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Sprites',$html,$lang,$pathPartsParsed);
}
?>
