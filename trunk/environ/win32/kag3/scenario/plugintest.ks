[iscript]

class TestPlugin extends KAGPlugin
{
	function TestPlugin
	{
		super.KAGPlugin();
	}

	function finalize()
	{
		super.finalize();
	}


	function onStore(f, elm)
	{
		dm("onStore", f, elm);
	}

	function onRestore(f, clear, elm)
	{
		dm("onRestore", f, clear, elm);
	}

	function onStableStateChanged(stable)
	{
		dm("onStableStateChanged", stable);
	}

	function onMessageHiddenStateChanged(hidden)
	{
		dm("onMessageHiddenStateChanged", hidden);
	}

	function onCopyLayer(toback)
	{
		dm("onCopyLayer", toback);
	}

	function onExchangeForeBack()
	{
		dm("onExchangeForeBack");
	}
}

dm("kag = "+ kag);
kag.addPlugin(new TestPlugin());

[endscript]



