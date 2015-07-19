/*
 important caveat when trying this with Web MIDI on Google Chrome: 
 You need to request SysEx access, which requires elevated permissions. 
 And by that I mean that your code won’t work from your local harddisk, 
 or, when loaded over the web, from non-secure connections. What worked 
 for me is loading it over a local web server using “localhost”.
 */

var midi = {

	midiAccess: null,  // global MIDIAccess object
	output: null,
	self: null,
	midiSuccessCallback: null,
	midiFailureCallback: null,
	
	onMIDISuccess: function ( midiAccessParam ) {
		console.log( "MIDI ready!" );
		self.midiAccess = midiAccessParam;  // store in the global (in real usage, would probably keep in an object instance)				
		if(self.midiSuccessCallback){
			self.midiSuccessCallback();
		}
	},

	onMIDIFailure: function (msg) {
		console.log( "Failed to get MIDI access - " + msg );
		if(self.midiFailureCallback) {
			self.midiFailureCallback();
		}
	},

	init: function(midiSuccessCallback, midiFailureCallback){
		self = this;
		self.midiSuccessCallback = midiSuccessCallback;
		self.midiFailureCallback = midiFailureCallback;
		navigator.requestMIDIAccess( { sysex: true } ).then( self.onMIDISuccess, self.onMIDIFailure );
	},

	listInputs: function() {
		for (var entry of self.midiAccess.inputs) {
		var input = entry[1];
		console.log( "Input port [type:'" + input.type + "'] id:'" + input.id +
			"' manufacturer:'" + input.manufacturer + "' name:'" + input.name +
			"' version:'" + input.version + "'" );
		}	
	},
	
	listOutputs: function() {
		for (var entry of self.midiAccess.outputs) {
		var output = entry[1];
		console.log( "Output port [type:'" + output.type + "'] id:'" + output.id +
			"' manufacturer:'" + output.manufacturer + "' name:'" + output.name +
			"' version:'" + output.version + "'" );
		}
	},

	selectOutput: function(outputID){
		self.output = self.midiAccess.outputs.get(outputID);		
	},

	sendSysex: function(address, value) {
		// sysex messages have to be sent in one go, so concatenate arrays
		var sysexBytes = 
			[0xF0]
			.concat(address)
			.concat(value)
			.concat([0xF7]);
			
		self.output.send(sysexBytes);
	},
	
	sendNoteOnC: function(){	
		var noteOnMessage = [0x90, 60, 0x7f];    // note on, middle C, full velocity  
		self.output.send( noteOnMessage );  //omitting the timestamp means send immediately.
	},
	
	sendNoteOffC: function(){	
		var noteOnMessage = [0x80, 60, 0x7f];    // note off, middle C
		self.output.send( noteOnMessage );  //omitting the timestamp means send immediately.
	}	
};