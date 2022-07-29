
class QuestItem extends MovieClip
{
	// Instances
	var Title:MovieClip;
	var ObjectiveItemList:Array;

	// References
	private var TitleTextField:TextField;

	// Variables
	private var bracketOpenLongestNameFrame:Number;

	public function QuestItem()
	{
		ObjectiveItemList = new Array();

		TitleTextField = Title.TitleTextField;
		TitleTextField.textAutoSize = "shrink";

		Title.Bracket.gotoAndStop("longest name");
		bracketOpenLongestNameFrame = Title.Bracket._currentframe;
	}

	public function SetQuestInfo(a_type:Number, a_title:String, a_objectives:Array):Void
	{
		// Set end piece art
		Title.EndPiece.gotoAndStop(a_type);

		// Set quest title as in the journal
		TitleTextField.text = a_title.toUpperCase();

		// Make room for the title
		var bracketOpenFrame:Number = Math.min(bracketOpenLongestNameFrame, Math.ceil(TitleTextField.textWidth * bracketOpenLongestNameFrame / TitleTextField._width));
		Title.Bracket.gotoAndStop(bracketOpenFrame);

		var yOffset = Title.Bracket._y + Title.Bracket._height;
		for (var i:Number = 0; i < a_objectives.length; i++)
		{
			var objectiveItem:MovieClip = attachMovie("ObjectiveItem", "objective" + i, getNextHighestDepth(), { _x:Title._x, _y: yOffset });

			objectiveItem.Objective.textField.text = a_objectives[i];

			ObjectiveItemList.push(objectiveItem);

			yOffset += 1.1 * objectiveItem._height;
		}
	}

	public function SetSide(a_side:String):Void
	{
		Title.EndPiece.SideArt.gotoAndStop(a_side);
	}

	public function Show():Void
	{
		gotoAndPlay("FadeIn");

		for (var i:Number = 0; i < ObjectiveItemList.length; i++)
		{
			ObjectiveItemList[i].gotoAndPlay("FadeIn");
		}
	}

	public function Remove():Void
	{
		for (var i:Number = 0; i < ObjectiveItemList.length; i++)
		{
			ObjectiveItemList[i].gotoAndPlay("FadeOut");
		}

		gotoAndPlay("FadeOut");
	}
}
