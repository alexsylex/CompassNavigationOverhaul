var questItem:QuestItem;
var entries:Array;

var All:Boolean;
var Favor:Boolean;
var DialogueMode:Boolean;
var StealthMode:Boolean;
var Swimming:Boolean;
var HorseMode:Boolean;
var WarHorseMode:Boolean;

function QuestItemList():Void
{
	entries = new Array();

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

function Test(a_questData:Object):Void
{
	//_level0.Test.TextField0.text = String(a_questData.type) + " " + a_questData.name + " " + a_questData.objective;
	_level0.Test.TextField0.text = entries.length;
}

function AddQuest(a_questType:Number, a_questName:String, a_questObjective:String):Void
{
	questItem = attachMovie("QuestItem", "questItem", getNextHighestDepth());

	questItem._y = (questItem._height + 5) * entries.length;

	entries.push(questItem);

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

function RemoveAllQuests():Void
{
	for (var i:Number = 0; i < entries.length; i++)
	{
		questItem = entries[i];
		questItem.Remove();
	}

	entries.splice(0, entries.length);
}
