var midi = {

	midi: null,  // global MIDIAccess object
	output: null,

    onMIDISuccess: function ( midiAccess ) {
		console.log( "MIDI ready!" );
		this.midi = midiAccess;  // store in the global (in real usage, would probably keep in an object instance)
	},

	onMIDIFailure: function (msg) {
		console.log( "Failed to get MIDI access - " + msg );
	},

	init: function(){
		navigator.requestMIDIAccess( { sysex: true } ).then( onMIDISuccess, onMIDIFailure );
	},

	listInputs: function() {
		for (var entry of this.midi.inputs) {
		var input = entry[1];
		console.log( "Input port [type:'" + input.type + "'] id:'" + input.id +
			"' manufacturer:'" + input.manufacturer + "' name:'" + input.name +
			"' version:'" + input.version + "'" );
		}	
	},
	
	listOutputs: function() {
		for (var entry of this.midi.outputs) {
		var output = entry[1];
		console.log( "Output port [type:'" + output.type + "'] id:'" + output.id +
			"' manufacturer:'" + output.manufacturer + "' name:'" + output.name +
			"' version:'" + output.version + "'" );
		}
	},

	selectOutput: function(outputID){
		this.output = this.midi.outputs.get(outputID);		
	},

	sendSysex: function(address, value) {
		this.output.send([0x70]);
		this.output.send(address);
		this.output.send(value);
		this.output.send([0x7f]);
	},
	
	sendNoteOnC: function(){	
		var noteOnMessage = [0x90, 60, 0x7f];    // note on, middle C, full velocity  
		this.output.send( noteOnMessage );  //omitting the timestamp means send immediately.
	}
};

var sysexAddress = [0, 43, 105];
var sysexData = [1,2,3,4,5,6];

midi.init();
midi.listOutputs();
midi.selectOutput(1);
midi.sendSysex(sysexAddress,sysexData);
