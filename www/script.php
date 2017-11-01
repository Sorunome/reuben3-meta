<?php

$defines['script_num_vars'] = 1;

function dechexpad($i, $size){
	return str_pad(dechex($i),$size,'0',STR_PAD_LEFT);
}

class Parser{
	private $bytes = 0;
	private $variables = [];
	private $offset = 0;
	private $out = '';
	private $functions = [];
	private $if_stack = [];
	private $while_stack = [];
	private $defines = [];
	private $extra_vars = ['camera_x', 'camera_y'];
	private $firstPass = true;
	private $labels = [];
	private $genericLabelCounter = 0;
	private $addressPrefix = '';
	private function getLabel(){
		return '++generic_label_'.($this->genericLabelCounter++).'++';
	}
	private function addLabel($label,$offset = 0){
		if(!$this->firstPass){
			return;
		}
		$this->labels[$this->addressPrefix.$label] = $this->bytes + $offset + $this->offset;
	}
	private function getAddress($label){
		if($this->firstPass){
			return '00000000';
		}
		return dechexpad($this->labels[$this->addressPrefix.$label], 8);
	}
	private function getVarNum($var){
		if(isset($this->variables[$var])){
			return dechexpad($this->variables[$var], 2);
		}
		if(($i = array_search($var,$this->extra_vars)) !== false){
			return dechexpad(0xFF - $i, 2);
		}
		var_dump("._:");
	}
	private function getVar($var){
		if(($s = $this->getVarNum($var))){
			return '80'.$s;
		}
		$var = (int)$var;
		if($var < 0){
			$var += 256;
		}
		if($var == 0x80){
			return '8080';
		}
		return dechexpad($var, 2);
	}
	private function parseLine($line,$convertToBytes = true){
		$out = '';
		$line = trim($line);
		$matches = [];
		if(!preg_match('/^(\\w+)(\\((?:[^()]*(?:\\([^)]*\\))?)*\\))?;?$/',$line,$matches)){
			return '';
		}
		$function = $matches[1];
		$args = [];
		if(isset($matches[2])){
			preg_match_all('/(?:\\(|,)([^(),]*(?:\\([^)]+[^),]*\\))?)/',$matches[2],$matches,PREG_SET_ORDER);
			foreach($matches as $m){
				$m[1] = strtr($m[1],$this->defines);
				if(isset($this->functions[$function]) && isset($this->functions[$function]['unparsed_args']) && $this->functions[$function]['unparsed_args']){
				}else{
					if(preg_match('/^[\d\s*+\\/\\-()]+$/',$m[1])){
						$m[1] = eval("return $m[1];");
					}
				}
				$args[] = trim($m[1]);
			}
		}
		if(isset($this->functions[$function])){
			$fn = $this->functions[$function];
			$numArgs = sizeof($args);
			if($numArgs >= $fn['args_min'] && $numArgs <= $fn['args_max']){
				$s = $fn['fn']($args);
				if($convertToBytes){
					$s = hex2bin($s);
				}
				$out .= $s;
			}
		}
		return $out;
	}
	public function __construct(){
		$this->functions = [
			'fade_to_white' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					return '01';
				}
			],
			'fade_from_white' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					return '02';
				}
			],
			'set_map' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					return '03'.$this->getVar($args[0]);
				}
			],
			'focus_cam' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					return '06';
				}
			],
			'update_screen' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					return '07';
				}
			],
			'set_var' => [
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args){
					$s = '08'.$this->getVarNum($args[0]).$this->getVar($args[1]);
					var_dump($s);
					return '08'.$this->getVarNum($args[0]).$this->getVar($args[1]);
				}
			],
			'label' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					$this->addLabel($args[0]);
				}
			],
			'goto' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					return '09'.$this->getAddress($args);
				}
			],
			'jump_ifnot' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args){
					return '0a';
				}
			],
			'lt' => [
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args){
					return '0b'.$this->getVar($args[0]).$this->getVar($args[1]);
				}
			],
			'gt' => [ // gt is like lt....with reversed arguments!
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args){
					return '0b'.$this->getVar($args[1]).$this->getVar($args[0]);
				}
			],
			'add' => [
				'args_min' => 2,
				'args_max' => 2,
				'fn' => function($args){
					return '0c'.$this->getVarNum($args[0]).$this->getVar($args[1]);
				}
			],
			'inc' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					return '0e'.$this->getVarNum($args[0]);
				}
			],
			'dec' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					return '0f'.$this->getVarNum($args[0]);
				}
			],
			'call' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					return '10'.$getAddress([$args[0]]);
				}
			],
			'ret' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args){
					return '11';
				}
			],
			'text' => [
				'args_min' => 1,
				'args_max' => 99999999999,
				'fn' => function($args){
					$s = implode(',',$args);
					$s = preg_replace_callback('/\\\\x([\da-fA-F]{2})/',function($matches){
						return hex2bin($matches[1]);
					},$s);
					$s = strtr($s,[
						'\\n' => "\xfe",
						'\\r' => "\xfd"
					]);
					$a = [];
					while(strlen($s)){
						if(strlen($s) > 19){
							for($i = 19;$i >= 0;$i--){
								if(isset($s[$i]) && $s[$i] == ' '){
									break;
								}
							}
							if($i <= 0){
								$i = 19;
							}
							$a[] = substr($s,0,$i);
							$s = ltrim(substr($s,$i));
						}else{
							$a[] = $s;
							break;
						}
					}
					$s = '';
					$alt = true;
					foreach($a as $e){
						$s .= $e;
						if($alt = !$alt){
							// pagebreak
							$s .="\xfd";
						}else{
							// linebreak
							$s .= "\xfe";
						}
					}
					$s = substr($s,0,-1);
					return '04'.bin2hex($s).'ff';
				}
			],
			'add_enemy' => [
				'args_min' => 3,
				'args_max' => 3,
				'fn' => function($args){
					return '05'.$this->getVar($args[0]).$this->getVar($args[1]).$this->getVar($args[2]);
				}
			],
			'hex' => [
				'args_min' => 1,
				'args_max' => 1,
				'unparsed_args' => true,
				'fn' => function($args){
					return $args[0];
				}
			],
			'put_address' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					return $this->getAddress($args[0]);
				}
			],
			'set_address_prefix' => [
				'args_min' => 0,
				'args_max' => 1,
				'fn' => function($args){
					if(sizeof($args) == 0){
						$this->addressPrefix = '';
					}else{
						$this->addressPrefix = $args[0];
					}
				}
			],
			
			'return_false' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args){
					return 'fe';
				}
			],
			'return_true' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function($args){
					return 'ff';
				}
			],
			
			'define_vars' => [
				'args_min' => 1,
				'args_max' => 10,
				'fn' => function($args){
					foreach($args as $i => $a){
						$this->variables[$a] = $i;
					}
				}
			],
			'clear_vars' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					$this->variables = [];
				}
			],
			'set_player_pos' => [
				'args_min' => 2,
				'args_max' => 5,
				'fn' => function($args){
					$out = '';
					$size = sizeof($args);
					$update = false;
					if($size > 2 && $args[$size-1] == 'update'){
						$update = true;
						$size--;
					}
					$focus = false;
					if($size > 2 && $args[$size-1] == 'focus'){
						$focus = true;
						$size--;
					}
					$i = 0;
					if($size > 2){
						$out .= $this->functions['set_map']['fn']([$args[$i++]]);
					}
					$out .= $this->functions['set_var']['fn'](['player_x',$args[$i++]]);
					
					$out .= $this->functions['set_var']['fn'](['player_y',$args[$i++]]);
					if($focus){
						$out .= '06';
					}
					if($update){
						$out .= '07';
					}
					return $out;
				}
			],
			'return' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					if($args[0] == 'true'){
						return 'ff';
					}
					return 'fe';
				}
			],
			'if' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					$out = '0a'.$this->parseLine($args[0],false);
					$label = $this->getLabel();
					$this->if_stack[] = $label;
					return $out.$this->getAddress($label);
				}
			],
			'else' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					$label = array_pop($this->if_stack);
					$this->addLabel($label,5);
					
					$out = '09';
					$label = $this->getLabel();
					$this->if_stack[] = $label;
					return $out.$this->getAddress($label);
				}
			],
			'endif' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					$label = array_pop($this->if_stack);
					$this->addLabel($label);
				}
			],
			'do' => [
				'args_min' => 0,
				'args_max' => 0,
				'fn' => function(){
					$label = $this->getLabel();
					$this->addLabel($label);
					$this->while_stack[] = $label;
				}
			],
			'while' => [
				'args_min' => 1,
				'args_max' => 1,
				'fn' => function($args){
					$out = '0d'.$this->parseLine($args[0],false);
					$label = array_pop($this->while_stack);
					$out .= $this->getAddress($label);
					return $out;
				}
			]
		];
	}
	public function parse($script,$offset = 0,$def = []){
		$this->defines = $def;
		$this->offset = $offset;
		$this->variables = [];
		$this->bytes = 0;
		$out = '';
		$this->if_stack = [];
		$this->while_stack = [];
		$this->labels = [];
		$this->firstPass = true;
		$this->genericLabelCounter = 0;
		$this->addressPrefix = '';
		foreach(explode("\n",strtolower($script)) as $line){
			$out .= $this->parseLine($line);
			$this->bytes = strlen($out);
		}
		$this->addressPrefix = '';
		$this->bytes = 0;
		$out = '';
		$this->firstPass = false;
		$this->genericLabelCounter = 0;
		foreach(explode("\n",strtolower($script)) as $line){
			$out .= $this->parseLine($line);
			$this->bytes = strlen($out);
		}
		global $defines;
		if(sizeof($this->variables) > $defines['script_num_vars']){
			$defines['script_num_vars'] = sizeof($this->variables);
		}
		return $out;
	}
}
$parser = new Parser();
