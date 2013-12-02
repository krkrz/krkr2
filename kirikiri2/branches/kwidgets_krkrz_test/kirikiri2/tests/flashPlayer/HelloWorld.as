package {
    import flash.display.*;  
    import flash.text.*;
	import flash.system.fscommand;
	import flash.external.ExternalInterface;
	import mx.utils.StringUtil;

    //HelloWorld
    [SWF(width=240, height=240, backgroundColor=0xFFFFFF)]
    public class HelloWorld extends Sprite {

		// コンストラクタ
		public function HelloWorld() {
			var textField:TextField=new TextField();
			textField.text="Hello World!";
			addChild(textField);
			fscommand("fscommand", "fsarg");
			ExternalInterface.marshallExceptions = true;
			ExternalInterface.addCallback("flashFunction", flashFunction);
			ExternalInterface.call("trace", ExternalInterface.call("tjsFunction", 10, "test"));
        }

		// ファンクション
		public function flashFunction(a:Number, b:String):String {
			return StringUtil.substitute("{0}:{1}", a, b);
		}
    }
}
