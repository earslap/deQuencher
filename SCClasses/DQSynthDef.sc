//types: 0-param 1-synth 2-t_trig 3-fx 4-chainFx 5-freeSounder
DQSynthDef
{
	var sType, sName, sFunc, fadeTime, t1proto, t3proto, t4proto, retSynth;
	
	*new
	{|argName, argFunc, argType, argFade|
		^super.new.init(argName, argFunc, argType, argFade);
	}
	
	init
	{|argName, argFunc, argType, argFade|
		
		if((argName.class != String).and(argName.class != Symbol),
		{
			"First argument 'name' needs to be a string or symbol...".postln;
			this.halt;
		});
		
		sName = argName;
		
		if((argType != 1).and(argType != 3).and(argType != 4),
		{
			"Third argument 'type' needs to be 1, 3 or 4...".postln;
			this.halt;
		});
		
		sType = argType;
		
		if(argFade == nil, { fadeTime = 0; }, { fadeTime = argFade; });
		
		if(argFunc.class != Function,
		{
			"Second argument must be a function...".postln;
			this.halt;
		});
		
		sFunc = argFunc;
		
		sType.switch(
		1,
		{
			retSynth =
			SynthDef(argName,
			{
				arg fxBusNo = 10, fxMul = 0, t_trig100 = 0;
				var freeEnv = EnvGen.ar(Env([1, 0], [fadeTime], 'welch'), t_trig100, doneAction: 2);
				var sound = SynthDef.wrap(sFunc);
				Out.ar(fxBusNo, sound * fxMul.lag(0.1) * freeEnv);
			});
		},
		3,
		{
			retSynth = 
			SynthDef(argName,
			{
				arg inFxBusNo = 16, t_trig100 = 0;
				var freeEnv = EnvGen.ar(Env([1, 0], [fadeTime], 'welch'), t_trig100, doneAction: 2); 
				var in = InFeedback.ar(inFxBusNo, 2); 
				var sound = SynthDef.wrap(sFunc, [0], [in]);
				Out.ar(0, sound * freeEnv);
			});			
		},
		4,
		{
			retSynth =
			SynthDef(argName,
			{
				arg inFxBusNo = 16, outFxBusNo = 1038, t_trig100 = 0, fxMul = 0;
           		var freeEnv = EnvGen.ar(Env([1, 0], [10], 'welch'), t_trig100, doneAction: 2); 
           		var in = InFeedback.ar(inFxBusNo, 2);
           		var sound = SynthDef.wrap(sFunc, [0], [in]);
           		Out.ar(outFxBusNo, sound * freeEnv * fxMul.lag(0.1)); 
			});
		}
		);
		
		SynthDef("fsplayer_stereo", //freesound player for stereo files.
         { 
			arg bufferNo, fxBusNo = 1038, fxMul = 0, t_trig100 = 0;
			var free = FreeSelf.kr(t_trig100);
                              
			var needle = Phasor.ar(0, BufRateScale.kr(bufferNo), 0, BufFrames.kr(bufferNo));
			var source = BufRd.ar(2, bufferNo, needle, 0, 2);
                              
			Out.ar(fxBusNo, source.dup * fxMul.lag(0.1));
         }).send(Server.default); 

		SynthDef("fsplayer_mono", //freesound player for mono files.
		{ 
			arg bufferNo, fxBusNo = 1038, fxMul = 0, t_trig100 = 0;
			var free = FreeSelf.kr(t_trig100); 
                              
			var needle = Phasor.ar(0, BufRateScale.kr(bufferNo), 0, BufFrames.kr(bufferNo));
			var source = BufRd.ar(1, bufferNo, needle, 0, 2);
                              
			Out.ar(fxBusNo, source.dup * fxMul.lag(0.1));
         }).send(Server.default); 

				
		^retSynth;
		
	}  
}