define("dojox/charting/action2d/Highlight", ["dojo/_base/lang", "dojo/_base/declare", "dojo/_base/Color", "dojo/_base/connect", "dojox/color/_base",
		"./PlotAction", "dojo/fx/easing", "dojox/gfx/fx"], 
	function(lang, declare, Color, hub, c, PlotAction, dfe, dgf){

	/*=====
	var __HighlightCtorArgs = {
		// summary:
		//		Additional arguments for highlighting actions.
		// duration: Number?
		//		The amount of time in milliseconds for an animation to last.  Default is 400.
		// easing: dojo/fx/easing/*?
		//		An easing object (see dojo.fx.easing) for use in an animation.  The
		//		default is dojo.fx.easing.backOut.
		// highlight: String|dojo/_base/Color|Function?
		//		Either a color or a function that creates a color when highlighting happens.
	};
	=====*/
	
	var DEFAULT_SATURATION  = 100,	// %
		DEFAULT_LUMINOSITY1 = 75,	// %
		DEFAULT_LUMINOSITY2 = 50,	// %
		cc = function(color){
			return function(){ return color; };
		},

		hl = function(color){
			var a = new c.Color(color),
				x = a.toHsl();
			if(x.s == 0){
				x.l = x.l < 50 ? 100 : 0;
			}else{
				x.s = DEFAULT_SATURATION;
				if(x.l < DEFAULT_LUMINOSITY2){
					x.l = DEFAULT_LUMINOSITY1;
				}else if(x.l > DEFAULT_LUMINOSITY1){
					x.l = DEFAULT_LUMINOSITY2;
				}else{
					x.l = x.l - DEFAULT_LUMINOSITY2 > DEFAULT_LUMINOSITY1 - x.l ?
						DEFAULT_LUMINOSITY2 : DEFAULT_LUMINOSITY1;
				}
			}
			var rcolor = c.fromHsl(x);
			rcolor.a = a.a;
			return rcolor;
		},

		spiderhl = function(color){
			var r = hl(color);
			r.a = 0.7;
			return r;
		}

	return declare("dojox.charting.action2d.Highlight", PlotAction, {
		// summary:
		//		Creates a highlighting action on a plot, where an element on that plot
		//		has a highlight on it.

		// the data description block for the widget parser
		defaultParams: {
			duration: 400,	// duration of the action in ms
			easing:   dfe.backOut	// easing for the action
		},
		optionalParams: {
			highlight: "red"	// name for the highlight color
								// programmatic instantiation can use functions and color objects
		},

		constructor: function(chart, plot, kwArgs){
			// summary:
			//		Create the highlighting action and connect it to the plot.
			// chart: dojox/charting/Chart
			//		The chart this action belongs to.
			// plot: String?
			//		The plot this action is attached to.  If not passed, "default" is assumed.
			// kwArgs: __HighlightCtorArgs?
			//		Optional keyword arguments object for setting parameters.
			var a = kwArgs && kwArgs.highlight;
			this.colorFunc = a ? (lang.isFunction(a) ? a : cc(a)) : hl;
			this.connect();
		},

		process: function(o){
			// summary:
			//		Process the action on the given object.
			// o: dojox/gfx/shape.Shape
			//		The object on which to process the highlighting action.
			if(!o.shape || !(o.type in this.overOutEvents)){ return; }

			// if spider let's deal only with poly
			if(o.element == "spider_circle" || o.element == "spider_plot"){
				return;
			}else if(o.element == "spider_poly" && this.colorFunc == hl){
				// hardcode alpha for compatibility reasons
				// TODO to remove in 2.0
				this.colorFunc = spiderhl;
			}

			var runName = o.run.name, index = o.index, anim;

			if(runName in this.anim){
				anim = this.anim[runName][index];
			}else{
				this.anim[runName] = {};
			}

			if(anim){
				anim.action.stop(true);
			}else{
				var color = o.shape.getFill();
				if(!color || !(color instanceof Color)){
					return;
				}
				this.anim[runName][index] = anim = {
					start: color,
					end:   this.colorFunc(color)
				};
			}

			var start = anim.start, end = anim.end;
			if(o.type == "onmouseout"){
				// swap colors
				var t = start;
				start = end;
				end = t;
			}

			anim.action = dgf.animateFill({
				shape:    o.shape,
				duration: this.duration,
				easing:   this.easing,
				color:    {start: start, end: end}
			});
			if(o.type == "onmouseout"){
				hub.connect(anim.action, "onEnd", this, function(){
					if(this.anim[runName]){
						delete this.anim[runName][index];
					}
				});
			}
			anim.action.play();
		}
	});
	
});
