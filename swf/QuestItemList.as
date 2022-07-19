// Instances
var QuestTitle:MovieClip;

function QuestItemList():Void
{
	var point:Object = { x:10, y:90 };
	globalToLocal(point);
	_x = point.x;
	_y = point.y;

	QuestTitle.TitleTextField.autoSize = "right";
}

function SetQuestInfo(a_questType:Number, a_questName:String, a_questObjective:String):Void
{
	QuestTitle.EndPiece.gotoAndStop(a_questType);
	QuestTitle.TitleTextField.text = a_questName.toUpperCase();
	QuestObjective.textField.text = a_questObjective;

	var bracketOpenFrame:Number = Math.round(QuestTitle.TitleTextField._width / 2.5);
	QuestTitle.Bracket.gotoAndStop(bracketOpenFrame);
}

function ShowQuestInfo():Void
{
	gotoAndPlay("FadeIn");
}

function HideQuestInfo():Void
{
	gotoAndPlay("FadeOut");
}
