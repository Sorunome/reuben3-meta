<?php
if(!$security->isLoggedIn() || !($user_info['power']&128)){
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("/");</script>Redirecting...',$lang,$pathPartsParsed);
	die();
}
$sql->switchDb('soru_reuben3_meta');
include_once(realpath(dirname(__FILE__)).'/functions.php');

if(isset($_GET['edit'])){
	$spritesheet = $sql->query("SELECT `id`,`name`,`sprites` FROM `spritesheets` WHERE `id`=%d",[(int)$_GET['edit']],0);
	$spritesheet['sprites'] = json_decode($spritesheet['sprites'],true);
	$html = '
	<style type="text/css">
		#spritesheetEdit {
			display:inline-block;
			width:'.(34*11).'px;
			vertical-align:top;
		}
		#spritesCont {
			display:inline-block;
			width:calc(100% - '.(34*11).'px);
			vertical-align:top;
		}
		#spritesheet {
			line-height: 0;
		}
		#cursprite {
			border: 1px solid red;
			display:inline-block;
		}
		.img {
			display:inline-block;
			width:32px;
			height:32px;
			background-image:url("/reuben3-meta/disp?spritesheet&amp;scale=4&amp;'.time().'");
			border:1px solid red;
			margin:0;
			padding:0;
		}
		.img.empty {
			background: none;
		}
	</style>
	<script type="text/javascript">
		$(document).ready(function(){
			var spritesheet = '.json_encode($spritesheet['sprites']['sprites']).',
				width = '.((int)$spritesheet['sprites']['width']).',
				height = '.((int)ceil(sizeof($spritesheet['sprites']['sprites']) / $spritesheet['sprites']['width'])).',
				curSprite = 1,
				pad = function(){
					while((spritesheet.length % width) > 0){
						spritesheet.push(-1);
					}
				},
				getBgPos = function(i){
					return "-"+(8*4*(i % 15)).toString()+"px -"+(8*4*Math.floor(i / 15)).toString()+"px";
				},
				drawBoard = function(){
					pad(); // make sure it looks neat!
					$("#spritesheet").empty().append(
						$.map(spritesheet,function(s,i){
							var $elem = $("<div>").addClass("img").css("background-position",getBgPos(s)).data("id",s);
							if(s == -1){
								$elem.addClass("empty");
							}
							$elem.mousedown(function(e){
								e.preventDefault();
								var $self = $(this);
								if(e.button == 2){
									// right click
									if($self.data("id") != -1){
										$self.data("id",-1);
										$self.addClass("empty");
										spritesheet[i] = -1;
									}
								}else{
									// left click
									$self.removeClass("empty");
									$self.data("id",curSprite);
									spritesheet[i] = curSprite;
									$self.css("background-position",getBgPos(curSprite));
								}
							});
							return $elem;
						})
					).css("width",width*34);
				},
				updateCursprite = function(i){
					curSprite = i;
					$("#cursprite").empty().append(
						$("<img>").attr("src","disp.php?scale=8&sprite="+curSprite.toString(10))
					);
					homepage.get("sprites.php?info="+curSprite.toString(10),function(data){
						$("#cursprite").append(
							$.map(data,function(v,i){
								return [
									"<br>",
									$("<b>").text(i),
									":",
									$("<span>").text(v)
								];
							})
						);
					});
				}
			updateCursprite(1);
			$("#spritesCont .img").click(function(e){
				e.preventDefault();
				updateCursprite(this.dataset.id);
			});
			$("#width").change(function(e){
				var val = parseInt(this.value,10);
				if(val > 10){
					val = 10;
				}
				if(val > width){
					var diff = val - width;
					for(var i = 0;i < diff;i++){
						for(var j = 0;j < height;j++){
							spritesheet.splice((width + i + 1)*j + (width + i),0,-1);
						}
					}
				}else{
					var diff = width - val;
					for(var j = 0;j < height;j++){
						spritesheet.splice((width - diff)*(j + 1),diff);
					}
				}
				width = val;
				drawBoard();
			});
			$("#height").change(function(e){
				var val = parseInt(this.value,10);
				if(val > height){
					var diff = val - height;
					for(var i = 0;i < diff * width;i++){
						spritesheet.push(-1);
					}
				}else{
					var diff = height - val;
					for(var i = 0;i < diff * width;i++){
						spritesheet.pop();
					}
				}
				height = val;
				drawBoard();
			});
			$("#save").click(function(e){
				e.preventDefault();
				console.log('.$spritesheet['id'].');
				console.log(spritesheet);
				console.log(width);
				console.log($("#spritesheet_name").val());
				homepage.post("?save='.((int)$spritesheet['id']).'",
				{
					"spritesheet":JSON.stringify(spritesheet),
					"width":width,
					"name":$("#spritesheet_name").val()
				},
				function(data){
					alert(data);
				});
			});
			drawBoard();
		});
	</script>
	<div id="spritesheetEdit">
		Name: <input type="text" id="spritesheet_name" value="'.htmlspecialchars($spritesheet['name']).'"><br>
		<div id="spritesheet" oncontextmenu="return false;"></div>
		Width: <input id="width" type="number" value="'.((int)$spritesheet['sprites']['width']).'"><br>
		Height: <input id="height" type="number" value="'.((int)ceil(sizeof($spritesheet['sprites']['sprites']) / $spritesheet['sprites']['width'])).'">
	</div><div id="spritesCont">Sprites:<br>
	<div id="cursprite"></div><br>
	';
	$sprites = $sql->query("SELECT `id` FROM `sprites` WHERE 1");
	foreach($sprites as $s){
		$html .= '<div class="img" data-id="'.$s['id'].'" style="background-position:-'.(8*4*($s['id'] % 15)).'px -'.(8*4*floor($s['id'] / 15)).'px"></div>';
	}
	$html .= '</div>
	<br>
	<button id="save">Save</button><br><br>
	<a href="spritesheet">&lt;&lt;Back</a>';
	
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Spritesheets',$html,$lang,$pathPartsParsed);
}elseif(isset($_GET['save'])){
	$sql->query("UPDATE `spritesheets` SET `name`='%s',`sprites`='%s' WHERE `id`=%d",[$_POST['name'],json_encode([
		'width' => (int)$_POST['width'],
		'sprites' => json_decode($_POST['spritesheet'],true)
	]),(int)$_GET['save']]);
	echo 'Saved';
}elseif(isset($_GET['new'])){
	$max = $sql->query("SELECT MAX(`sorder`) AS `max` FROM `spritesheets`",[],0);
	$sql->query("INSERT INTO `spritesheets` (`sprites`,`sorder`) VALUES ('%s',%d)",[json_encode([
		'width' => 4,
		'sprites' => []
	]),$max['max'] + 1]);
	$sql->switchDb('soru_homepage');
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("spritesheet");</script>Redirecting...',$lang,$pathPartsParsed);
}else{
	$spritesheets = $sql->query("SELECT `id`,`name` FROM `spritesheets` WHERE 1");
	$html = '<h2>Spritesheet editor</h2>';
	foreach($spritesheets as $s){
		$name = (string)$s['id'];
		if($s['name']){
			$name = $s['name'];
		}
		$html .= '<a href="?edit='.$s['id'].'">'.$name.'</a><br>';
	}
	$html .= '<hr><a href="?new">New</a><br><a href=".">&lt;&lt;Back</a>';

	$sql->switchDb('soru_homepage');
	echo $page->getPage('Spritesheets',$html,$lang,$pathPartsParsed);
}
