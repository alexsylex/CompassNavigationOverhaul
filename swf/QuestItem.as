
class QuestItem extends MovieClip
{
	// Instances
	var QuestTitle:MovieClip;
	var QuestObjective:MovieClip;

	// References
	private var QuestTitleTextField:TextField;

	// Variables
	private var bracketOpenLongestNameFrame:Number;

	public function QuestItem()
	{
		QuestTitleTextField = QuestTitle.TitleTextField;

		QuestTitle.Bracket.gotoAndStop("longest name");
		bracketOpenLongestNameFrame = QuestTitle.Bracket._currentframe;
		QuestTitle.Bracket.gotoAndStop(1);

		QuestTitleTextField.textAutoSize = "shrink";
	}

	public function SetQuestInfo(a_questType:Number, a_questName:String, a_questObjective:String):Void
	{
		QuestTitle.EndPiece.gotoAndStop(a_questType);
		QuestTitleTextField.text = a_questName.toUpperCase();
		var bracketOpenFrame:Number = Math.min(bracketOpenLongestNameFrame, Math.ceil(QuestTitleTextField.textWidth * bracketOpenLongestNameFrame / QuestTitleTextField._width));
		QuestTitle.Bracket.gotoAndStop(bracketOpenFrame);

		QuestObjective.textField.text = a_questObjective;
	}

	public function SetSide(a_side:String):Void
	{
		QuestTitle.EndPiece.SideArt.gotoAndStop(a_side);
	}

	public function Show():Void
	{
		gotoAndPlay("FadeIn");
	}

	public function Remove():Void
	{
		gotoAndPlay("FadeOut");
	}
}
