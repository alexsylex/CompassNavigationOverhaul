var questItem:QuestItem;

var All:Boolean;
var Favor:Boolean;
var DialogueMode:Boolean;
var StealthMode:Boolean;
var Swimming:Boolean;
var HorseMode:Boolean;
var WarHorseMode:Boolean;

function QuestItemList():Void
{
	var point:Object = { x:10, y:90 };
	globalToLocal(point);
	_x = point.x;
	_y = point.y;

	_level0.HUDMovieBaseInstance.HudElements.push(this);

	All = true;
	Favor = true;
	DialogueMode = true;
	StealthMode = true;
	Swimming = true;
	HorseMode = true;
	WarHorseMode = true;
}

function AddQuest(a_questType:Number, a_questName:String, a_questObjective:String):Void
{
	questItem = attachMovie("QuestItem", "questItem", getNextHighestDepth());

	questItem.SetQuestInfo(a_questType, a_questName, a_questObjective);
}

function SetQuestSide(a_side:String):Void
{
	questItem.SetSide(a_side);
}

function ShowQuest():Void
{
	questItem.Show();
}

function RemoveQuest():Void
{
	questItem.Remove();
}
