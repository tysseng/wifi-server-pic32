var OUTPUTMODE_BLOCK_MIDI = 0;
var OUTPUTMODE_REVERT_TO_MIDI = 1;
var OUTPUTMODE_INSTANT_SWITCH = 2;

var SYSEX_OP_WRITE_SETTINGS_TO_EE = 1;
var SYSEX_OP_CLEAR_SETTINGS_FROM_EE = 2;
var SYSEX_OP_CHANGE_SETTING = 3;

var MIDI_SYSEX_START = 0xF0;
var MIDI_SYSEX_END = 0xF7;

var sysexAddress = [0, 43, 102];
var midiChannel = 0;

var defaultsettings = [
  //Default midi mapping
  //switches (16)
  72,  // SW_A_RANGE
  73,  // SW_A_WAVE
  82,  // SW_A_LFO
  81,  // SW_A_ENV
  74,  // SW_B_RANGE
  75,  // SW_B_WAVE
  80,  // SW_B_LFO
  79,  // SW_B_ENV
  76,  // SW_SYNC
  77,  // SW_VCF_POL
  78,  // SW_VCA
  85,  // SW_CHORUS
  84,  // SW_DCO_POL
  83,  // SW_LFO_WAVE
  126, // SW_MANUAL
  127, // SW_WRITE

  //Pots (18)
  12, // POT_B_FINE
  13, // POT_B_TUNE
  14, // POT_DCO_ENV
  15, // POT_DCO_LFO
  16, // POT_MIX
  17, // POT_HPF
  18, // POT_RESONANCE
  19, // POT_CUTOFF
  20, // POT_VCF_ENV
  21, // POT_VCF_LFO
  22, // POT_PITCH_FOLLOW
  23, // POT_LEVEL
  24, // POT_LFO_RATE
  25, // POT_LFO_DELAY
  26, // POT_A 30
  27, // POT_D 31
  28, // POT_S 32
  29, // POT_R 33
  
  1, // Save settings to EE prom after sysex update.
  0, // default midi channel
  OUTPUTMODE_REVERT_TO_MIDI, //default output mode (but is overwritten by the switch detector)
  1, // Remove realtime midi and midi not destined for the JX-3P
  4, //DEFAULT_SWITCH_TO_MIDI_TIMER_OVERFLOWS - 20MHz: 4 timeouts approx 50ms delay after PG-200 before switching to midi (+ another 26ms before midi can be sent).

  //two pole switch boundary
  64,
  
  //three pole switch boundaries
  42,
  84,
  
  //four pole switch boundaries
  32,
  64,
  96
];

function populateFields(settings){

  // all input fields
  $('input[pos]').each(function(){
    var field = $(this);
    if(field.attr('type') === 'checkbox'){
	  if(settings[field.attr('pos')] == 0){
	    field.prop("checked", false);
	  } else {
	    field.prop("checked", true);
	  }
	} else {
      field.val(settings[field.attr('pos')]);
	}
  });
  
  // all dropdowns
  $('select[pos]').each(function(){
    $(this).val(settings[$(this).attr('pos')]);
  });
  
}

function generateSettings(){

	var settings = new Array(defaultsettings.length);

	// all input fields
	$('input').each(function(){
		if($(this).attr('pos')){
			var field = $(this);
			if(field.attr('type') === 'checkbox'){
				if(field.prop("checked")){
					settings[field.attr('pos')] = 1;
				} else {
					settings[field.attr('pos')] = 0;
				}
			} else {
				settings[field.attr('pos')] = parseInt(field.val());
			}
		}
	});
  
	// all dropdowns
	$('select').each(function(){
		if($(this).attr('pos')){
			settings[$(this).attr('pos')] = parseInt($(this).val());
		}
	});
	return settings;
}

function downloadSettings() {

	var settings = generateSettings();
    var bufferSize = settings.length*2 + 6;

	var buffer = new ArrayBuffer(bufferSize);
	var data = new DataView(buffer);

	// sysex start
	data.setUint8 (0, MIDI_SYSEX_START);
	
	// sysex address
	data.setUint8 (1, sysexAddress[0]);
	data.setUint8 (2, sysexAddress[1]);
	data.setUint8 (3, sysexAddress[2]);
	
	// sysex function
	data.setUint8 (4, SYSEX_OP_CHANGE_SETTING);
	
	// settings
	for(i = 0; i<settings.length; i++){
	  data.setUint8 (i*2+5, i);
	  data.setUint8 (i*2+6, settings[i]);
	}
	// sysex end
	data.setUint8 (bufferSize - 1, MIDI_SYSEX_END);
	
	var blob = new Blob([data], {type: 'application/octet-binary'})
	saveAs(blob, "mpg200-settings.syx");
}

function downloadClearSettings() {

    var bufferSize = 6;

	var buffer = new ArrayBuffer(bufferSize);
	var data = new DataView(buffer);

	// sysex start
	data.setUint8 (0, MIDI_SYSEX_START);
	
	// sysex address
	data.setUint8 (1, sysexAddress[0]);
	data.setUint8 (2, sysexAddress[1]);
	data.setUint8 (3, sysexAddress[2]);
	
	// sysex function
	data.setUint8 (4, SYSEX_OP_CLEAR_SETTINGS_FROM_EE);
	
	// sysex end
	data.setUint8 (5, MIDI_SYSEX_END);
	
	var blob = new Blob([data], {type: 'application/octet-binary'})
	saveAs(blob, "mpg200-clear-settings.syx");
}

function midiReadyCallback() {
	var dropdown = $("#outputDropdown");
	for (var entry of midi.midiAccess.outputs) {
		var outputs = entry[1];
		dropdown
			.append($("<option></option>")
			.attr("value",outputs.id)
			.text(outputs.name)); 		
	}        
	dropdown.val(0);
	setMidiDeviceFromDropdown();
	setMidiChannelFromDropdown();
}
          
function setMidiDeviceFromDropdown(){
	var dropdown = $("#outputDropdown");
	var deviceId = dropdown.val();
	console.log("set midi device to " + deviceId);
	midi.selectOutput(deviceId);
}		  
	
function setMidiChannelFromDropdown(){
	midiChannel = $("#midichannel").val();
	console.log("set midi channel to " + midiChannel);
}		
	
function sendSettingsAsSysex(){
	var sysexData = [SYSEX_OP_CHANGE_SETTING].concat(generateSettings());
	midi.sendSysex(sysexAddress,sysexData);
}

function sendClearSettingsAsSysex(){
	var sysexData = [SYSEX_OP_CLEAR_SETTINGS_FROM_EE];
	midi.sendSysex(sysexAddress,sysexData);
}

function sendDialValueAsCC(value, dial){
	value = Math.round(value);
	var pos = $(dial.i).attr("pos-link"); 
	var cc = $("[pos='"+pos+"']").val();

	midi.sendCC(midiChannel, cc, value);
}

//kickstart everything on document load
function init(){
	populateFields(defaultsettings);	  
	  
	$(".dial127").knob({
		'angleArc': 270,
		'angleOffset': 225,
		'min': 0,
		'max': 127,
		'step': 1,
		"width": 50,
		"height": 50,
		"fgColor": "#ff0000",
		"thickness": ".2",			
		'change' : function (v) { 
			sendDialValueAsCC(v, this);
		},
		'release' : function (v) { 
			sendDialValueAsCC(v, this);
		}
		
	});
	$(".dial127").val(0);
	
	midi.init(midiReadyCallback);
}

$(init);