<?php
if(!$security->isLoggedIn() || !($user_info['power']&32)){
	echo $page->getPage('Nope','<script type="text/javascript">getPageJSON("/");</script>Redirecting...',$lang,$pathPartsParsed);
	die();
}
$sql->switchDb('soru_reuben3_meta');
include_once(realpath(dirname(__FILE__)).'/functions.php');

if(isset($_GET['getData'])){
	header('Content-Type: text/json');
	$map = $sql->query("SELECT `data`,`id`,`area`,`enemies` FROM `tilemaps` WHERE `x`=%d AND `y`=%d and `mapId`=%d",[(int)$_GET['x'],(int)$_GET['y'],(int)$_GET['getData']],0);
	$tl = $sql->query("SELECT `id` FROM `tilemaps` WHERE `x`=%d AND `Y`=%d and `mapId`=%d",[(int)$_GET['x']-1,(int)$_GET['y']-1,(int)$_GET['getData']],0);
	$t = $sql->query("SELECT `id` FROM `tilemaps` WHERE `x`=%d AND `Y`=%d and `mapId`=%d",[(int)$_GET['x'],(int)$_GET['y']-1,(int)$_GET['getData']],0);
	$tr = $sql->query("SELECT `id` FROM `tilemaps` WHERE `x`=%d AND `Y`=%d and `mapId`=%d",[(int)$_GET['x']+1,(int)$_GET['y']-1,(int)$_GET['getData']],0);
	$l = $sql->query("SELECT `id` FROM `tilemaps` WHERE `x`=%d AND `Y`=%d and `mapId`=%d",[(int)$_GET['x']-1,(int)$_GET['y'],(int)$_GET['getData']],0);
	$r = $sql->query("SELECT `id` FROM `tilemaps` WHERE `x`=%d AND `Y`=%d and `mapId`=%d",[(int)$_GET['x']+1,(int)$_GET['y'],(int)$_GET['getData']],0);
	$bl = $sql->query("SELECT `id` FROM `tilemaps` WHERE `x`=%d AND `Y`=%d and `mapId`=%d",[(int)$_GET['x']-1,(int)$_GET['y']+1,(int)$_GET['getData']],0);
	$b = $sql->query("SELECT `id` FROM `tilemaps` WHERE `x`=%d AND `Y`=%d and `mapId`=%d",[(int)$_GET['x'],(int)$_GET['y']+1,(int)$_GET['getData']],0);
	$br = $sql->query("SELECT `id` FROM `tilemaps` WHERE `x`=%d AND `Y`=%d and `mapId`=%d",[(int)$_GET['x']+1,(int)$_GET['y']+1,(int)$_GET['getData']],0);
	
	$info = [];
	$info['id'] = $map['id'];
	if($vars->get('defaultMap') == $map['id']){
		$info['Default'] = 'yes';
	}
	$mapJSON = array_pad([],12*8,1);
	$dynTiles = [];
	$eventTiles = [];
	if($map['data']!==NULL){
		$mapJSON = json_decode($map['data'],true);
		$res = $sql->query("SELECT `id`,`x`,`y`,`event`,`newTile` FROM `dynTiles` WHERE `refId`=%d",[$map['id']]);
		if($res[0]['id'] != NULL){
			$dynTiles = $res;
		}
		$res = $sql->query("SELECT `id`,`x`,`y` FROM `eventTiles` WHERE `refId`=%d",[$map['id']]);
		if($res[0]['id'] != NULL){
			$eventTiles = $res;
		}
	}
	$json = [
		'map' => $mapJSON,
		'tl' => $tl['id'],
		't' => $t['id'],
		'tr' => $tr['id'],
		'l' => $l['id'],
		'r' => $r['id'],
		'bl' => $bl['id'],
		'b' => $b['id'],
		'br' => $br['id'],
		'info' => $info,
		'dynTiles' => $dynTiles,
		'eventTiles' => $eventTiles,
		'area' => (int)$map['area'],
		'enemies' => $map['enemies']!==NULL?(bool)$map['enemies']:true
	];
	echo json_encode($json);
}elseif(isset($_GET['save'])){
	$x = (int)$_GET['x'];
	$y = (int)$_GET['y'];
	$mapId = (int)$_GET['save'];
	$map = $_POST['map'];
	$enemies = (int)$_POST['enemies'];
	header('Content-Type: text/plain');
	$m = $sql->query("SELECT `id` FROM `tilemaps` WHERE `x`=%d AND `y`=%d AND `mapId`=%d",[$x,$y,$mapId],0);
	if($m['id']!==NULL){
		$sql->query("UPDATE `tilemaps` SET `data`='%s',`area`=%d,`enemies`=%d WHERE `id`=%d",[$map,(int)$_GET['area'],$enemies,(int)$m['id']]);
	}else{
		$sql->query("INSERT INTO `tilemaps` (`data`,`x`,`y`,`mapId`,`enemies`) VALUES ('%s',%d,%d,%d,%d)",[$map,$x,$y,$mapId,$enemies]);
	}
	$vars->set('map'.(string)$mapId.'_curx',$x);
	$vars->set('map'.(string)$mapId.'_cury',$y);
	echo "Saved";
}elseif(isset($_GET['saveDynTile'])){
	$sql->query("UPDATE `dynTiles` SET `x`=%d,`y`=%d,`event`=%d,`newTile`=%d WHERE `id`=%d",[(int)$_POST['x'],(int)$_POST['y'],(int)$_POST['event'],(int)$_POST['newTile'],(int)$_GET['saveDynTile']]);
	echo "Saved dyn tile";
}elseif(isset($_GET['delDynTile'])){
	$sql->query("DELETE FROM `dynTiles` WHERE `id`=%d",[(int)$_GET['delDynTile']]);
}elseif(isset($_GET['getEventTile'])){
	$t = $sql->query("SELECT `id`,`code`,`x`,`y`,`add_jump` FROM `eventTiles` WHERE `id`=%d",[(int)$_GET['getEventTile']],0);
	header('Content-Type: text/json');
	echo json_encode($t);
}elseif(isset($_GET['saveEventTile'])){
	$sql->query("UPDATE `eventTiles` SET `x`=%d,`y`=%d,`code`='%s',`add_jump`=%d WHERE `id`=%d",[(int)$_POST['x'],(int)$_POST['y'],$_POST['code'],(int)$_POST['add_jump'],(int)$_GET['saveEventTile']]);
	echo "Saved event tile";
}elseif(isset($_GET['delEventTile'])){
	$sql->query("DELETE FROM `eventTiles` WHERE `id`=%d",[(int)$_GET['delEventTile']]);
}elseif(isset($_GET['newDynTile'])){
	$map = $sql->query("SELECT `id` FROM `tilemaps` WHERE `x`=%d AND `y`=%d and `mapId`=%d",[(int)$_GET['x'],(int)$_GET['y'],(int)$_GET['newDynTile']],0);
	if($map['id']!==NULL){
		$sql->query("INSERT INTO `dynTiles` (`refId`,`x`,`y`,`event`) VALUES (%d,%d,%d,%d)",[(int)$map['id'],(int)$_POST['x'],(int)$_POST['y'],(int)$_POST['event']]);
	}
}elseif(isset($_GET['newEventTile'])){
	$map = $sql->query("SELECT `id` FROM `tilemaps` WHERE `x`=%d AND `y`=%d and `mapId`=%d",[(int)$_GET['x'],(int)$_GET['y'],(int)$_GET['newEventTile']],0);
	if($map['id']!==NULL){
		$sql->query("INSERT INTO `eventTiles` (`refId`,`x`,`y`) VALUES (%d,%d,%d)",[(int)$map['id'],(int)$_POST['x'],(int)$_POST['y']]);
	}
}elseif(isset($_GET['delete'])){
	$sql->query("DELETE FROM `tilemaps` WHERE `x`=%d AND `y`=%d AND `mapId`=%d",[(int)$_GET['x'],(int)$_GET['y'],(int)$_GET['delete']]);
}elseif(isset($_GET['move'])){
	$x = (int)$_GET['x'];
	$y = (int)$_GET['y'];
	$mapId = (int)$_GET['move'];
	$xo = (int)$_GET['xo'];
	$yo = (int)$_GET['yo'];
	$nx = $x + $xo;
	$ny = $y + $yo;
	$id = (int)$sql->query("SELECT `id` FROM `tilemaps` WHERE `x`=%d AND `y`=%d AND `mapId`=%d",[$x,$y,$mapId],0)['id'];
	$sql->query("UPDATE `tilemaps` SET `x`=%d,`y`=%d WHERE `x`=%d AND `y`=%d AND `mapId`=%d",[$x,$y,$nx,$ny,$mapId]);
	$sql->query("UPDATE `tilemaps` SET `x`=%d,`y`=%d WHERE `id`=%d",[$nx,$ny,$id]);
}elseif(isset($_GET['edit'])){
?>
<!DOCTYPE html>
<html>
	<head>
		<title>Reuben 3 tilemapper</title>
		<script type="text/javascript" src="/jquery-2.0.3.min.js"></script>
		<script type="text/javascript" src="/homepage.js.php"></script>
		<style type="text/css">
			body,html{
				background-color:#3D7031;
			}
			body > div {
				vertical-align: top;
			}
			#tilemaps {
				display:inline-block;
				padding:0;
				margin:0;
				line-height:0;
				width:1152px;
			}
			#tilemaps > div {
				display:inline-block;
				padding:0;
				margin:0;
				line-height:0;
				width:384px;
				height:256px;
			}
			div#tilemap {
				width:384px;
				height:256px;
				border:1px solid red;
				margin:-1px;
				position:relative;
				z-index:10;
			}
			#sprites {
				display:inline-block;
				width:350px;
			}
			#sprites .img {
				border:1px solid red;
				margin:0;
				padding:0;
			}
			#current {
				display:inline-block;
				width:200px;
			}
			#current > div {
				border:1px solid red;
			}
			.spritesheet .toggleContent {
				line-height:0;
			}
			#rightCont {
				display:inline-block;
			}
			.dynTile,.eventTile {
				position:absolute;
				z-index:10;
				margin:0;
				padding:0;
				border-style:none;
				width:32px;
				height:32px;
				opacity:0.5;
				cursor:pointer;
			}
			.dynTile {
				background-color:#14A327;
			}
			.dynTile.current,.eventTile.current {
				opacity:0.8;
			}
			.eventTile {
				background-color:#BF2C3D;
			}
			#eventTilesEditCont textarea {
				width:100%;
				height:400px;
			}
		</style>
		<script type="text/javascript">
			$(document).ready(function(){
					var sprites = <?php
							$spritesheets = $sql->query("SELECT `name`,`sprites` FROM `spritesheets` WHERE 1 ORDER BY `sorder` ASC");
							$sheets = [];
							$spritesInUse = [];
							foreach($spritesheets as $sheet){
								$sheets[$sheet['name']] = json_decode($sheet['sprites'],true);
								foreach($sheets[$sheet['name']]['sprites'] as $s){
									$spritesInUse[] = $s;
								}
							}
							$allSprites = $sql->query("SELECT `id` FROM `sprites` WHERE 1");
							$extraSprites = [];
							foreach($allSprites as $s){
								if(!in_array((int)$s['id'],$spritesInUse)){
									$extraSprites[] = (int)$s['id'];
								}
							}
							$sheets['Misc'] = [
									'width' => 10,
									'sprites' => $extraSprites
								];
							echo json_encode($sheets);
						?>,
						tilemap = [],
						dynTiles = [],
						eventTiles = [],
						curSprite = 1,
						changed = false,
						enemies = false,
						all_areas = <?php echo json_encode($sql->query("SELECT `id`,`name` FROM `areas` WHERE 1"))?>,
						area = 0,
						x = <?php echo ($vars->get('map'.(int)$_GET['edit'].'_curx') | 0) ?>,
						y = <?php echo ($vars->get('map'.(int)$_GET['edit'].'_cury') | 0) ?>,
						mapId = <?php echo (int)$_GET['edit'];?>,
						spriteSheetURL = 'disp.php?spritesheet&scale=4&<?=time()?>',
						getTilePosition = function(id){
							return '-'+(8*4*(id % 15)).toString()+'px -'+(8*4*Math.floor(id / 15)).toString()+'px';
						},
						getTileElem = function(id){
							if(id == -1){
								return $('<div>').css({
									display:'inline-block',
									width:8*4,
									height:8*4
								});
							}
							return $('<div>').css({
								display:'inline-block',
								width:8*4,
								height:8*4,
								backgroundImage:'url("'+$('<span>').text(spriteSheetURL).html()+'")',
								backgroundPosition:getTilePosition(id)
							}).addClass('img');
						},
						loadTileMap = function(){
							homepage.get('tilemaps.php?getData='+mapId.toString(10)+'&x='+x.toString(10)+'&y='+y.toString(10),function(data){
								tilemap = data.map;
								changed = false;
								dynTiles = data.dynTiles;
								eventTiles = data.eventTiles;
								area = data.area;
								enemies = data.enemies;
								$('#tilemap').empty().append(
									$.map(tilemap,function(tile,i){
										return getTileElem(tile)
											.data('id',i)
											.click(function(e){
												if(!$('#toggleDynTiles').data('show') && !$('#toggleEventTiles').data('show')){
													var i = $(this).data('id');
													e.preventDefault();
													tilemap[i] = curSprite;
													$(this).css('background-position',getTilePosition(curSprite));
													changed = true;
												}
											});
									})
								);
								data.info.x = x.toString(10);
								data.info.y = y.toString(10);
								$('#currentMap').empty().append(
									$('<u>').text('Tilemap'),
									'<br>Enemies:',$('<input>').attr('type','checkbox').attr(enemies?'checked':'false','checked').change(function(){
										enemies = this.checked;
										changed = true;
									}),
									'<br>Area:',$('<select>').css('max-width','100%').append(
										$.map(all_areas,function(a){
											return $('<option>').val(a.id).text(a.name).attr((a.id == area?'selected':'false'),'selected');
										})
									).change(function(){area = parseInt(this.value,10);changed = true;}),
									$.map(data.info,function(v,i){
										return [
											'<br>',
											$('<b>').text(i),
											':',
											$('<span>').text(v)
										]
									})
								);
								
								$('#tileMapTL').empty();
								if(data.tl!==null){
									$('#tileMapTL').append($('<img>').attr('src','disp.php?scale=4&map='+data.tl+'&'+(new Date()).getTime()));
								}
								$('#tileMapT').empty();
								if(data.t!==null){
									$('#tileMapT').append($('<img>').attr('src','disp.php?scale=4&map='+data.t+'&'+(new Date()).getTime()));
								}
								$('#tileMapTR').empty();
								if(data.tr!==null){
									$('#tileMapTR').append($('<img>').attr('src','disp.php?scale=4&map='+data.tr+'&'+(new Date()).getTime()));
								}
								$('#tileMapL').empty();
								if(data.l!==null){
									$('#tileMapL').append($('<img>').attr('src','disp.php?scale=4&map='+data.l+'&'+(new Date()).getTime()));
								}
								$('#tileMapR').empty();
								if(data.r!==null){
									$('#tileMapR').append($('<img>').attr('src','disp.php?scale=4&map='+data.r+'&'+(new Date()).getTime()));
								}
								$('#tileMapBL').empty();
								if(data.bl!==null){
									$('#tileMapBL').append($('<img>').attr('src','disp.php?scale=4&map='+data.bl+'&'+(new Date()).getTime()));
								}
								$('#tileMapB').empty();
								if(data.b!==null){
									$('#tileMapB').append($('<img>').attr('src','disp.php?scale=4&map='+data.b+'&'+(new Date()).getTime()));
								}
								$('#tileMapBR').empty();
								if(data.br!==null){
									$('#tileMapBR').append($('<img>').attr('src','disp.php?scale=4&map='+data.br+'&'+(new Date()).getTime()));
								}
								if($('#toggleDynTiles').data('show')){
									showDynTiles();
								}else{
									hideDynTiles();
								}
								if($('#toggleEventTiles').data('show')){
									showEventTiles();
								}else{
									hideEventTiles();
								}
							});
						},
						save = function(fn){
							if(changed){
								homepage.post('tilemaps.php?save='+mapId.toString(10)+'&x='+x.toString(10)+'&y='+y.toString(10)+'&area='+area.toString(10),
								{
									map:JSON.stringify(tilemap),
									enemies:enemies?1:0
								},
								function(data){
									changed = false;
									alert(data);
									if(fn!==undefined){
										fn();
									}
								});
							}else if(fn!==undefined){
								fn();
							}
						},
						showDynTiles = function(){
							homepage.get('tilemaps.php?getData='+mapId.toString(10)+'&x='+x.toString(10)+'&y='+y.toString(10),function(data){
								hideEventTiles();
								dynTiles = data.dynTiles;
								$('.dynTile').remove();
								$('#newDynTileButton').css('display','');
								console.log(dynTiles);
								$('#tilemap').append(
									$.map(dynTiles,function(tile){
										var newTile = $.extend({},tile);
										return $('<div>').addClass('dynTile').css({
												top:newTile.y*32,
												left:newTile.x*32
											}).click(function(e){
												var _self = this;
												$('.dynTile').removeClass('current');
												$(this).addClass('current');
												$('#dynTilesEditCont').empty().append(
													$.map({
															'id':newTile.id,
															'x':newTile.x,
															'y':newTile.y,
															'event':newTile.event
														},function(v,i){
														return [
															'<br>',
															$('<b>').text(i),
															':',
															$('<span>').text(v)
														]
													}),'<br>',
													$('<b>').text('new tile:'),
													$('<img>').attr('src','disp.php?scale=6&sprite='+newTile.newTile).click(function(){
														newTile.newTile = curSprite;
														$(this).attr('src','disp.php?scale=6&sprite='+curSprite.toString(10));
													}),'<br>',
													$('<button>').text('Save').click(function(e){
														e.preventDefault();
														homepage.post('tilemaps.php?saveDynTile='+newTile.id,{
															'x':newTile.x,
															'y':newTile.y,
															'event':newTile.event,
															'newTile':newTile.newTile
														},function(data){
															alert(data);
														})
													}),
													$('<button>').text('Delete').click(function(e){
														e.preventDefault();
														if(confirm('Are you sure you want to delete this dynamic tile?')){
															homepage.get('tilemaps.php?delDynTile='+newTile.id,function(){
																$(_self).remove();
																$('#dynTilesEditCont').empty();
															});
														}
													})
												);
											});
									})
								);
							});
						},
						hideDynTiles = function(){
							$('.dynTile').remove();
							$('#dynTilesEditCont').empty();
							$('#newDynTileButton').css('display','none');
							$('#toggleDynTiles').text('Show').data('show',false);
						},
						showEventTiles = function(){
							homepage.get('tilemaps.php?getData='+mapId.toString(10)+'&x='+x.toString(10)+'&y='+y.toString(10),function(data){
								hideDynTiles();
								eventTiles = data.eventTiles;
								$('.eventTile').remove();
								$('#newEventTileButton').css('display','');
								$('#tilemap').append(
									$.map(eventTiles,function(tile){
										var newTile = $.extend({},tile);
										return $('<div>').addClass('eventTile').css({
												top:newTile.y*32,
												left:newTile.x*32
											}).click(function(e){
												var _self = this;
												$('.eventTile').removeClass('current');
												$(this).addClass('current');
												homepage.get('tilemaps.php?getEventTile='+newTile.id.toString(10),function(data){
													newTile = $.extend(newTile,data);
													$('#eventTilesEditCont').empty().append(
														$.map({
																'id':newTile.id,
																'x':newTile.x,
																'y':newTile.y
															},function(v,i){
															return [
																'<br>',
																$('<b>').text(i),
																':',
																$('<span>').text(v)
															]
														}),'<br>',
														$('<label>').append('Add Return: ',$('<input>').attr('type','checkbox').attr(parseInt(newTile.add_jump,10)?'checked':'false','checked').change(function(e){
															newTile.add_jump = this.checked?1:0;
														})),'<br>',
														$('<textarea>').val(newTile.code).keydown(function(e){
															if(e.keyCode==9){
																e.preventDefault();
																var myValue = "\t";
																var startPos = this.selectionStart;
																var endPos = this.selectionEnd;
																var scrollTop = this.scrollTop;
																this.value = this.value.substring(0, startPos) + myValue + this.value.substring(endPos,this.value.length);
																this.focus();
																this.selectionStart = startPos + myValue.length;
																this.selectionEnd = startPos + myValue.length;
																this.scrollTop = scrollTop
															}
														}).change(function(e){
															newTile.code = this.value;
														}),'<br>',
														$('<button>').text('Save').click(function(e){
															e.preventDefault();
															homepage.post('tilemaps.php?saveEventTile='+newTile.id,{
																'x':newTile.x,
																'y':newTile.y,
																'code':newTile.code,
																'add_jump':newTile.add_jump
															},function(data){
																alert(data);
															})
														}),
														$('<button>').text('Delete').click(function(e){
															e.preventDefault();
															if(confirm('Are you sure you want to delete this event tile?')){
																homepage.get('tilemaps.php?delEventTile='+newTile.id,function(){
																	$(_self).remove();
																	$('#eventTilesEditCont').empty();
																});
															}
														})
													);
												});
											});
									})
								);
							});
						},
						hideEventTiles = function(){
							$('.eventTile').remove();
							$('#eventTilesEditCont').empty();
							$('#newEventTileButton').css('display','none');
							$('#toggleEventTiles').text('Show').data('show',false);
						},
						setCurSprite = function(id){
							curSprite = id;
							$('#currentSprite').empty().append(
								$('<img>').attr('src','disp.php?scale=8&sprite='+curSprite.toString(10))
							);
							homepage.get('sprites.php?info='+curSprite.toString(10),function(data){
								$('#currentSprite').append(
									$.map(data,function(v,i){
										return [
											'<br>',
											$('<b>').text(i),
											':',
											$('<span>').text(v)
										]
									})
								);
							});
						};
					$('#sprites').append(
						$.map(sprites,function(spd,sps){
							return $('<div>')
								.text(sps)
								.addClass('spritesheet')
								.append(' ',
									$('<a>')
										.text('+')
										.attr('href','http://toggle')
										.click(function(e){
											e.preventDefault();
											$(this).parent().find('.toggleContent').toggle();
										})
								)
								.css('width',spd.width*(8*4+2))
								.append(
									$('<div>')
										.css('display','none')
										.addClass('toggleContent')
										.append(
											$.map(spd.sprites,function(sp){
												return getTileElem(sp)
													.data('id',sp)
													.click(function(e){
														e.preventDefault();
														if($(this).data('id') == -1){
															return;
														}
														setCurSprite($(this).data('id'));
														curSprite = $(this).data('id');
													})
											})
										)
								);
						})
					);
					setCurSprite(1);
					$('#save').click(function(e){
						e.preventDefault();
						save();
					});
					$('#delete').click(function(e){
						e.preventDefault();
						if(confirm('Are you sure you want to delete this tilemap?')){
							homepage.get('tilemaps.php?delete='+mapId.toString(10)+'&x='+x.toString(10)+'&y='+y.toString(10),function(data){
								loadTileMap();
							});
						}
					});
					$('#tileMapTL').click(function(e){
						save();
						x--;
						y--;
						loadTileMap();
					});
					$('#tileMapT').click(function(e){
						save();
						y--;
						loadTileMap();
					});
					$('#tileMapTR').click(function(e){
						save();
						x++;
						y--;
						loadTileMap();
					});
					$('#tileMapL').click(function(e){
						save();
						x--;
						loadTileMap();
					});
					$('#tileMapR').click(function(e){
						save();
						x++;
						loadTileMap();
					});
					$('#tileMapBL').click(function(e){
						save();
						x--;
						y++;
						loadTileMap();
					});
					$('#tileMapB').click(function(e){
						save();
						y++;
						loadTileMap();
					});
					$('#tileMapBR').click(function(e){
						save();
						x++;
						y++;
						loadTileMap();
					});
					$('#moveUp').click(function(e){
						e.preventDefault();
						save(function(){
							homepage.get('tilemaps.php?move='+mapId.toString(10)+'&x='+x.toString(10)+'&y='+y.toString(10)+'&xo=0&yo=-1',function(data){
								y--;
								loadTileMap();
							})
						});
					});
					$('#moveDown').click(function(e){
						e.preventDefault();
						save(function(){
							homepage.get('tilemaps.php?move='+mapId.toString(10)+'&x='+x.toString(10)+'&y='+y.toString(10)+'&xo=0&yo=1',function(data){
								y++;
								loadTileMap();
							})
						});
					});
					$('#moveLeft').click(function(e){
						e.preventDefault();
						save(function(){
							homepage.get('tilemaps.php?move='+mapId.toString(10)+'&x='+x.toString(10)+'&y='+y.toString(10)+'&xo=-1&yo=0',function(data){
								x--;
								loadTileMap();
							})
						});
					});
					$('#moveRight').click(function(e){
						e.preventDefault();
						save(function(){
							homepage.get('tilemaps.php?move='+mapId.toString(10)+'&x='+x.toString(10)+'&y='+y.toString(10)+'&xo=1&yo=0',function(data){
								x++;
								loadTileMap();
							})
						});
					});
					$('#toggleDynTiles').data('show',false).click(function(e){
						e.preventDefault();
						if($(this).data('show')){
							hideDynTiles();
						}else{
							$(this).text('Hide').data('show',true);
							showDynTiles();
						}
					});
					$('#newDynTileButton').click(function(e){
						e.preventDefault();
						var dx = prompt('x'),
							dy = prompt('y'),
							ev = prompt('event');
						homepage.post('tilemaps.php?newDynTile='+mapId.toString(10)+'&x='+x.toString(10)+'&y='+y.toString(10),{
							'x':dx,
							'y':dy,
							'event':ev
						},function(data){
							showDynTiles();
						});
					});
					$('#toggleEventTiles').data('show',false).click(function(e){
						e.preventDefault();
						if($(this).data('show')){
							$(this).text('Show').data('show',false);
							hideEventTiles();
						}else{
							$(this).text('Hide').data('show',true);
							showEventTiles();
						}
					});
					$('#newEventTileButton').click(function(e){
						e.preventDefault();
						var dx = prompt('x'),
							dy = prompt('y');
						homepage.post('tilemaps.php?newEventTile='+mapId.toString(10)+'&x='+x.toString(10)+'&y='+y.toString(10),{
							'x':dx,
							'y':dy
						},function(data){
							showEventTiles();
						});
					});
					loadTileMap();
			});
		</script>
	</head>
	<body>
		<div id="tilemaps">
			<div id="tileMapTL"></div><div id="tileMapT"></div><div id="tileMapTR"></div>
			<div id="tileMapL"></div><div id="tilemap"></div><div id="tileMapR"></div>
			<div id="tileMapBL"></div><div id="tileMapB"></div><div id="tileMapBR"></div>
		</div>
		<div id="sprites"></div>
		<div id="current">
			<div id="currentSprite"></div>
			<div id="currentMap"></div>
			<div id="dynTiles">
				<u>DynTiles: <a href="http://toggle" id="toggleDynTiles">Show</a></u><br>
				<a href="http://new" id="newDynTileButton" style="display:none;">New</a>
				<div id="dynTilesEditCont"></div>
			</div>
			<div id="eventTiles">
				<u>EventTiles: <a href="http://toggle" id="toggleEventTiles">Show</a></u><br>
				<a href="http://new" id="newEventTileButton" style="display:none;">New</a>
				<div id="eventTilesEditCont"></div>
			</div>
		</div>
		<div id="rightCont">
			<button id="save">Save</button><br>
			<button id="delete">Delete</button><br>
			<a href="http://move" id="moveUp">Move Up</a><br>
			<a href="http://move" id="moveDown">Move Down</a><br>
			<a href="http://move" id="moveLeft">Move Left</a><br>
			<a href="http://move" id="moveRight">Move Right</a><br>
			<a href="tilemaps.php">&lt;&lt; Back</a>
		</div>
	</body>
</html>
<?php
}elseif(isset($_GET['newMap'])){
	$sql->query("INSERT INTO `maps` (`name`) VALUES ('%s')",[$_GET['newMap']]);
	$id = $sql->insertId();
	$vars->set('map'.(string)$id.'_curx',$x);
	$vars->set('map'.(string)$id.'_cury',$y);
}else{
	$html = '<div id="mapsCont">';
	$maps = $sql->query("SELECT `id`,`name` FROM `maps` WHERE 1");
	foreach($maps as $m){
		$html .= '<a href="tilemaps?edit='.$m['id'].'">'.$m['name'].'</a> Whole image: <a href="disp?world='.$m['id'].'" target="_blank">click</a><br>';
	}
	$html .= '</div><hr><a id="newLink">New</a><br>
	<a href="/reuben3-meta">&lt;&lt; Back</a>
	<script type="text/javascript">
		$("#newLink").click(function(e){
			e.preventDefault();
			var name = prompt("Name:");
			if(name != "" && name != null && name != undefined && name){
				homepage.get("/reuben3-meta/tilemaps?newMap="+encodeURIComponent(name),function(data){
					location.reload();
				});
			}
		});
	</script>';
	
	$sql->switchDb('soru_homepage');
	echo $page->getPage('List Tilemaps',$html,$lang,$pathPartsParsed);
}
