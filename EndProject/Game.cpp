#include"Game_Manager.h"
/*=============main_function==============*/
//遊戲主程式
void Main_Game(fstream& File_Character,fstream& File_Monster,fstream& File_Map, int DEBUG_MODE)
{
	vector<Character> Base_Character;	//角色模板，，用於之後創建角色清單時從裡面複製角色資料
	vector<Ethnicity> Monster;			//所有種族
	vector<Character> play_Character;	//玩家選擇的角色列表

	read_Character_Data(File_Character, Base_Character);	//Character讀檔
	read_Monster_Data(File_Monster, Monster);			//Monster讀檔
	string start;
	do {
		cout << "請輸入play以開始遊戲，或是輸入exit退出：";
		std::cin >> start;
	} while (start != "play" && start != "exit");
	if (start == "exit")
		return;
	while (1) 
	{
		Map GameMap; //所有Map
		creat_Character(Base_Character, play_Character);		//創建角色
		cout << endl;
		read_Map_Data(File_Map, GameMap, Monster, play_Character.size()); //Map讀檔
		//get_All_Base_Character_Data(play_Character);	//檢查Character資料
		get_All_Base_Monster_Data(Monster);				//檢查Monster資料
		//get_int_Map(GameMap);	//檢查地圖資料
		choose_Start_Position(play_Character, Monster, GameMap);	//選擇起始位置
		check_Monsters_Active(Monster, GameMap);	//檢查怪物狀態
		//開始遊戲主流程
		main_Battle(play_Character, Monster, GameMap,DEBUG_MODE);
		play_Character.clear();
		for (int i = 0; i < Monster.size(); i++) 
		{
			Monster[i].Creature_List.clear();
			Monster[i].Shuffle_Mark = false;
			for (int j = 0; j < Monster[i].Deck.size(); j++) 
				Monster[i].Deck[j].status = 0;
		}
		GameMap.~Map();
		string again;
		do {
			cout << "是否選擇再遊玩一次？(play/exit)";
			std::cin >> again;
		} while (again != "play" && again != "exit");
		if (again == "exit")
			break;
	}
}
void creat_Character(vector<Character>& Base_Character, vector<Character>& play_Character)
{
	char character_count;
	do {
		cout << "請輸入出場角色數量(請輸入2~4的正整數，我拜託你了):";	//輸入角色數量
		std::cin >> character_count;
		if ((character_count - '0') < 2 || (character_count - '0') > 4)
			cout << "數量錯誤!" << endl;
	} while ((character_count - '0') < 2 || (character_count - '0') > 4);
	for (int i = 0; i < character_count - '0'; i++)
	{
		cout << "***請輸入第"<< i + 1 << "隻角色***：";
		int character_number = 0;
		bool right_Character = false;
		do
		{	
			string character_name;
			std::cin >> character_name;
			for (int j = 0; j < Base_Character.size(); j++)
			{
				if (character_name == Base_Character[j].Character_name)		//找到模板中的角色
				{
					right_Character = true;
					character_number = j;
					break;
				}
			}
			if (right_Character == false) 
			{
				cout << "找不到此角色，請重新輸入..." << endl;
			}
		} while (right_Character == false);

		Character newCharacter = Base_Character[character_number];		//複製模板中的資料
		newCharacter.ID = 'A' + i;
		for (int j = 0; j < newCharacter.Hand; j++)
		{
			bool wrong_Input = false;
			int active_Card_ID;
			do {
				cout << "請輸入第" << j + 1 << "張卡片：";
				wrong_Input = false;
				std::cin >> active_Card_ID;
				if (active_Card_ID >= newCharacter.Deck.size() || active_Card_ID < 0) 
				{
					wrong_Input = true;
				}
				else 
				{
					if (newCharacter.Deck[active_Card_ID].status == 1)
					{
						wrong_Input = true;
					}
				}
				if (wrong_Input) 
				{
					cout << "輸入錯誤!" << endl;
				}
					
			} while (wrong_Input);
			newCharacter.Deck[active_Card_ID].status = 1;	//設定起始的手牌
		}
		play_Character.push_back(newCharacter);
	}
}
bool compare(Position a,Position b) 
{
	if (a.y > b.y)
		return false;
	else if (a.y == b.y && a.x > b.x)
		return false;
	else
		return true;
}
void choose_Start_Position(vector<Character>& play_Character, vector<Ethnicity>& Monster, Map& Game_Map)
{
	sort(Game_Map.Init_Pos.begin(), Game_Map.Init_Pos.end(),compare);
	for (int i = 0; i < Game_Map.Init_Pos.size(); i++) 
	{
		Game_Map.check_road(Game_Map.Init_Pos[i].x, Game_Map.Init_Pos[i].y);
	}
	Game_Map.print_Map(play_Character, Monster);
	//cin.ignore();
	for (int i = 0; i < play_Character.size(); i++) 
	{
		cout << endl << "***請輸入"<< play_Character[i].ID << "的起始位置***(請輸入wasd以選擇):";
		string position_input;
		Position start_point;
		start_point = Game_Map.Init_Pos[0];
		//此段似乎有bug引起輸入錯誤 而且不知道發生原因 我之後再測看看 下次測試幫我看看這裡cin會不會跳過
		std::cin >> position_input;
		for (int j = 0; j < position_input.size(); j++)
		{
			switch (position_input[j])
			{
			case'w':
			case 'W':
				if (Game_Map.Game_Map[start_point.y - 1][start_point.x] == 5) 
				{
					start_point.y--;
				}
				break;
			case's':
			case 'S':
				if (Game_Map.Game_Map[start_point.y + 1][start_point.x] == 5)
				{
					start_point.y++;
				}
				break;
			case'a':
			case 'A':
				if (Game_Map.Game_Map[start_point.y][start_point.x - 1] == 5)
				{
					start_point.x--;
				}
				break;
			case'd':
			case 'D':
				if (Game_Map.Game_Map[start_point.y][start_point.x + 1] == 5)
				{
					start_point.x++;
				}
				break;
			case'e':
			case 'E':
				break;
			default:
				break;
			}
			//if (position_input[j] == 13)	//enter鍵結束
			//	break;
		}
		play_Character[i].position.y = start_point.y;
		play_Character[i].position.x = start_point.x;
		Game_Map.Game_Map[start_point.y][start_point.x] = 1;
		for (int j = 0; j < Game_Map.Init_Pos.size(); j++) 
		{
			if (Game_Map.Init_Pos[j].x == start_point.x && Game_Map.Init_Pos[j].y == start_point.y) 
			{
				Game_Map.Init_Pos.erase(Game_Map.Init_Pos.begin() + j);
			}
		}
		if (i == play_Character.size() - 1)
			break;
		Game_Map.print_Map(play_Character, Monster);
	}
	for (int i = 0; i < Game_Map.High; i++) 
	{
		for (int j = 0; j < Game_Map.Width; j++)
		{
			if (Game_Map.Game_Map[i][j]==5) 
			{
				Game_Map.Game_Map[i][j] = 1;
			}
			
		}
	}
	Game_Map.print_Map(play_Character, Monster);
}
void main_Battle(vector<Character>& play_Character, vector<Ethnicity>& Monster, Map& Game_Map, int DEBUG_MODE)
{
	bool* already_played = nullptr;
	srand(time(NULL));
	int round = 1;
	while(end_Game(play_Character,Monster,Game_Map,false,round))	//結束遊戲條件 1.角色數量 怪物數量 門數量
	{
		//get_All_Base_Monster_Data(Monster);
		cout << "請選擇角色手牌:" << endl;
		already_played = new bool[play_Character.size()];
		for (int i = 0; i < play_Character.size(); i++) {
			already_played[i] = false;
			//cout << already_played[i] << endl;
		}
		//玩家角色準備
		do {
			string who;
			std::cin >> who;
			for (int i = 0; i < play_Character.size(); i++) {
				if (who[0] == play_Character[i].ID) {
					int Discard_num = calculate_Discard(play_Character[i]); //計算棄牌數
					string command; // 若為-1 check時為指令 其他則為 出牌的第一張
					std::cin >> command;
					if (command == "-1"&&Discard_num>=2&& !already_played[i]) {
						play_Character[i].Command[0] = -1;
						play_Character[i].Rest = true;
						already_played[i] = true;
					}
					else if (command=="check") {
						check(play_Character[i]);
					}
					else if(IsPlayHandCard(play_Character[i],command) && !already_played[i]) {
						int card2;
						std::cin >> card2;
						if (IsCardInHand(play_Character[i], card2)) {
							for (int j = 0; j < 2; j++) 
							{
								for (int k = 0; k < play_Character[i].Deck.size(); k++) 
								{
									if (play_Character[i].Deck[k].ID == command[0] - '0')
										play_Character[i].Command[0] = play_Character[i].Deck[k].ID;
									else if(play_Character[i].Deck[k].ID == card2)
										play_Character[i].Command[1] = play_Character[i].Deck[k].ID;
								}
							}
							already_played[i] = true;
						}
						else {
							break;
						}
					}
					else {
						cout << "Wrong Command" << endl;
					}
					break;
				}
				else if (who == "check") {
					print_character_sort_onActive_monster(Monster, play_Character);
					break;
				}
				/*else {
					cout << "Enter CharacterId and choose card" << endl;
				}*/
			}
		} while (check_player_done(already_played, play_Character.size()));
		delete[] already_played;
		//怪物準備
		for (int i = 0; i < Monster.size(); i++)	//第i個種族
		{
			bool all_deactivate = true;
			for (int j = 0; j < Monster[i].Creature_List.size(); j++) 
			{
				if (Monster[i].Creature_List[j].active == true)
					all_deactivate = false;
			}
			if (all_deactivate == true) 
			{
				Monster[i].Command = -1;
				continue;
			}
			if (DEBUG_MODE == 0) 
			{
				bool finished = false;
				do
				{
					int choose = rand() % (Monster[i].Deck.size());
					if (Monster[i].Deck[choose].status == 0)	//確認是否在牌組中
					{
						Monster[i].Deck[choose].status = 2;	//改成出牌中
						Monster[i].Command = choose;
						if (Monster[i].Deck[choose].Shuffle_Mark) {
							Monster[i].Shuffle_Mark = true;
						}
						finished = true;		//結束選擇
					}
				} while (finished == false);		//結束選擇即跳出此迴圈
			}
			else 
			{
				for (int j = 0; j < Monster[i].Deck.size(); j++) 
				{
					if (Monster[i].Deck[j].status == 0) 
					{
						Monster[i].Deck[j].status = 2;	//改成出牌中
						Monster[i].Command = j;
						if (Monster[i].Deck[j].Shuffle_Mark) {
							Monster[i].Shuffle_Mark = true;
						}
						break;
					}
				}
			}
		}
		//check_all_dex(play_Character,Monster);	//檢查敏捷值
		//判斷出手順序
		vector<char> attack_Sort;	//以敏捷值排序的攻擊順序
		//先排角色
		attack_Sort.push_back(play_Character[0].ID);	//將角色A加入排序
		for (int i = 1; i < play_Character.size(); i++)
		{
			for (int j = 0; j < attack_Sort.size(); j++)
			{
				int fir_Dex, sec_Dex;
				if (play_Character[i].Command[0] == -1)
				{
					fir_Dex = 99; sec_Dex = 99;
				}
				else
				{
					fir_Dex = play_Character[i].Deck[play_Character[i].Command[0]].Dexterity_Value;
					sec_Dex = play_Character[i].Deck[play_Character[i].Command[1]].Dexterity_Value;
				}
				if (fir_Dex < get_Character_Dex(play_Character, attack_Sort[j], 0))
				{
					attack_Sort.insert(attack_Sort.begin() + j, play_Character[i].ID);
					break;
				}
				else if (fir_Dex == get_Character_Dex(play_Character, attack_Sort[j], 0))
				{
					if (sec_Dex < get_Character_Dex(play_Character, attack_Sort[j], 1))
					{
						attack_Sort.insert(attack_Sort.begin() + j, play_Character[i].ID);
						break;
					}
					else if (j + 1 == attack_Sort.size()) //第二個敏捷值仍然相同或較大，由於是照角色字母順序加進排序，所以直接跳到下一位判斷，如果此位是最後一位，則直接將角色加到排序最後一位
					{
						attack_Sort.push_back(play_Character[i].ID);
						break;
					}
					else {
						continue;
					}
				}
				else if (j + 1 == attack_Sort.size()) //排序中的最後一位
				{
					attack_Sort.push_back(play_Character[i].ID);
					break;
				}
				else
				{

				}
			}
		}
		//再排怪物		由於怪物是以種族來排，所以序列裡我將怪物的代號以('a'+i代替)，到時候寫攻擊時要稍微注意
		for (int i = 0; i < Monster.size(); i++)
		{
			if (Monster[i].Command == -1)
				continue;
			/*bool all_active = true;
			for (int k = 0; k < Monster[i].Creature_List.size(); k++)
			{
				if (Monster[i].Creature_List[k].active == false)
					all_active = false;
			}*/
			for (int j = 0; j < attack_Sort.size(); j++)
			{
				if (attack_Sort[j] >= 'A' && attack_Sort[j] <= 'Z') //判斷是否是玩家角色
				{
					if (Monster[i].Deck[Monster[i].Command].Dexterity_Value < get_Character_Dex(play_Character, attack_Sort[j], 0))
					{
						attack_Sort.insert(attack_Sort.begin() + j, 'a' + i);
						break;
					}
				}
				else
				{
					if (Monster[i].Deck[Monster[i].Command].Dexterity_Value < get_Monster_Dex(Monster, attack_Sort[j]))
					{
						attack_Sort.insert(attack_Sort.begin() + j, 'a' + i);
						break;
					}
					else if (Monster[i].Deck[Monster[i].Command].Dexterity_Value == get_Monster_Dex(Monster, attack_Sort[j]) && 'a' + i < attack_Sort[j])
					{
						attack_Sort.insert(attack_Sort.begin() + j, 'a' + i);
						break;
					}
				}
				if (j + 1 == attack_Sort.size())
				{
					attack_Sort.push_back('a' + i);
					break;
				}
			}
			//}
		}
		//檢查排序
		cout << endl << "*******攻擊排序*******" << endl << endl;
		show_AttackList(attack_Sort, play_Character, Monster);
		cout << endl << "*****動作執行階段*****" << endl;
		//動作執行
		for (int i = 0; i < attack_Sort.size(); i++)
		{
			if (attack_Sort[i] <= 'Z' && attack_Sort[i] >= 'A')		//角色行動
			{
				for (int j = 0; j < play_Character.size(); j++) 
				{
					if (play_Character[j].ID == attack_Sort[i]) 
					{
						players_round(play_Character, play_Character[j], Monster, Game_Map);
					}
				}
			}
			else //怪物行動
			{
				for (int k = 0; k < Monster[attack_Sort[i]-'a'].Creature_List.size(); k++)
				{
					if(Monster[attack_Sort[i]-'a'].Creature_List[k].active == true)
						monsters_round(play_Character, Monster[attack_Sort[i] - 'a'], Monster[attack_Sort[i] - 'a'].Creature_List[k], Game_Map,Monster,attack_Sort);
					if(DEBUG_MODE == 0)
						_getch();
				}
			}
		}
		//回合結算
		attack_Sort.clear();
		end_round(play_Character, Monster, Game_Map);
		round++;
	}
}
bool end_Game(vector<Character>& play_Character, vector<Ethnicity>& Monster, Map& Map,bool UI_mode,int round) 
{
	bool all_Monsters_Dead = true;
	for (int i = 0; i < Monster.size(); i++)
	{
		if (Monster[i].Creature_List.size() != 0)
		{
			all_Monsters_Dead = false;
		}
	}
	bool all_Door_open = true;
	for (int i = 0; i < Map.High; i++)
	{
		for (int j = 0; j < Map.Width; j++)
		{
			if (Map.Game_Map[i][j] == 3)
			{
				all_Door_open = false;
			}
		}
	}
	if (all_Monsters_Dead == true && all_Door_open == true)
	{
		if (UI_mode == false)
			cout << "character win~" << endl;
		else
		{
			system("cls");
			cout << "character win~" << endl;
		}
		return false;
	}
	cout << "*************round " << round << "************" << endl << endl;
	for (int i = play_Character.size() - 1; i >= 0; i--)
	{
		int can_use_card = 0, can_throw_card = 0;
		for (int j = 0; j < play_Character[i].Deck.size(); j++)
		{
			if (play_Character[i].Deck[j].status == 1)
				can_use_card++;
			if (play_Character[i].Deck[j].status == 2)
				can_throw_card++;
		}
		if (can_use_card < 2 && can_throw_card < 2)
		{
			cout << play_Character[i].ID << " is killed!!" << endl;
			play_Character.erase(play_Character.begin() + i);
			Map.print_Map(play_Character, Monster);
		}
	}
	if (play_Character.size() == 0) 
	{
		if(UI_mode == false)
			cout << "monster win~" << endl;
		else
		{
			system("cls");
			cout << "monster win~" << endl;
		}
		return false;
	}
	return true;
}
//行動
void players_round(vector<Character>& play_Character, Character& Character, vector<Ethnicity>& Monster, Map& Game_Map)
{
	if (Character.Rest == false)
	{
		cout << endl << Character.ID << "'s rturn: " << endl << "card " << Character.Command[0] << "	上：";
		for (int k = 0; k < Character.Deck[Character.Command[0]].MovementUp.size(); k++)
		{
			cout << Character.Deck[Character.Command[0]].MovementUp[k].Movement << " " << Character.Deck[Character.Command[0]].MovementUp[k].Movement_Value << " ";
			if (Character.Deck[Character.Command[0]].MovementUp[k].Movement == "attack")
				cout << "range " << Character.Deck[Character.Command[0]].MovementUp[k].range << " ";
		}
		cout << " | 下：";
		for (int k = 0; k < Character.Deck[Character.Command[0]].MovementDown.size(); k++)
		{
			cout << Character.Deck[Character.Command[0]].MovementDown[k].Movement << " " << Character.Deck[Character.Command[0]].MovementDown[k].Movement_Value << " ";
			if (Character.Deck[Character.Command[0]].MovementDown[k].Movement == "attack")
				cout << "range " << Character.Deck[Character.Command[0]].MovementDown[k].range << " ";
		} 
		cout << endl << "card " << Character.Command[1] << "	上：";
		for (int k = 0; k < Character.Deck[Character.Command[1]].MovementUp.size(); k++)
		{
			cout << Character.Deck[Character.Command[1]].MovementUp[k].Movement << " " << Character.Deck[Character.Command[1]].MovementUp[k].Movement_Value << " ";
			if (Character.Deck[Character.Command[1]].MovementUp[k].Movement == "attack")
				cout << "range " << Character.Deck[Character.Command[1]].MovementUp[k].range << " ";
		}
		cout << " | 下：";
		for (int k = 0; k < Character.Deck[Character.Command[1]].MovementDown.size(); k++)
		{
			cout << Character.Deck[Character.Command[1]].MovementDown[k].Movement << " " << Character.Deck[Character.Command[1]].MovementDown[k].Movement_Value << " ";
			if (Character.Deck[Character.Command[1]].MovementDown[k].Movement == "attack")
				cout << "range " << Character.Deck[Character.Command[1]].MovementDown[k].range << " ";
		}cout << endl;
		int card_num=0; char UpOrDown=' ';
		string temp;
		cout << endl << "請輸入動作指令:";
		while (cin >> temp) {
			if (temp.size() == 2) {
				card_num = temp[0] - 48;
				UpOrDown = temp[1];
			}
			if (temp == "check") {
				print_character_sort_onActive_monster(Monster, play_Character);
			}
			else if (Character.Deck[card_num].status == 4&& temp.size() == 2)
			{
				if (UpOrDown == 'u'&& temp.size() == 2)
				{
					for (int j = 0; j < Character.Deck[card_num].MovementUp.size(); j++)
					{
						if (Character.Deck[card_num].MovementUp[j].Movement == "attack") {
							if (Character.Deck[card_num].MovementUp[j].range == 0)
							{
								if (Character.Attack(Character.Deck[card_num].MovementUp[j].Movement_Value, Monster) == true)
									Game_Map.print_Map(play_Character, Monster);
							}
							else
							{
								if (Character.Attack(Character.Deck[card_num].MovementUp[j].Movement_Value, Character.Deck[card_num].MovementUp[j].range, Monster, Game_Map) == true)
									Game_Map.print_Map(play_Character, Monster);
							}
						}
						else if (Character.Deck[card_num].MovementUp[j].Movement == "shield") {
							Character.Skill_shield(Character.Deck[card_num].MovementUp[j].Movement_Value);
							cout << Character.ID << " shield " << Character.Deck[card_num].MovementUp[j].Movement_Value << " this turn" << endl;
						}
						else if (Character.Deck[card_num].MovementUp[j].Movement == "heal") {
							Character.Skill_heal(Character.Deck[card_num].MovementUp[j].Movement_Value);
							cout << Character.ID << " heal " << Character.Deck[card_num].MovementUp[j].Movement_Value << ", now is " << Character.Hp << endl;
						}
						else if (Character.Deck[card_num].MovementUp[j].Movement == "move") {
							character_move(Character, Character.Deck[card_num].MovementUp[j].Movement_Value, Game_Map, play_Character, Monster);
							Game_Map.print_Map(play_Character, Monster);
						}
					}
					Character.Deck[card_num].status = 2;
					for (int j = 0; j < Character.Deck.size(); j++) {
						if (Character.Deck[j].status == 4 && Character.Deck[j].ID != card_num) {
							card_num = Character.Deck[j].ID;
							break;
						}
					}
					for (int j = 0; j < Character.Deck[card_num].MovementDown.size(); j++)
					{
						if (Character.Deck[card_num].MovementDown[j].Movement == "attack") {
							if (Character.Deck[card_num].MovementDown[j].range == 0)
							{
								if (Character.Attack(Character.Deck[card_num].MovementDown[j].Movement_Value, Monster) == true)
									Game_Map.print_Map(play_Character, Monster);
							}
							else
							{
								if (Character.Attack(Character.Deck[card_num].MovementDown[j].Movement_Value, Character.Deck[card_num].MovementDown[j].range, Monster, Game_Map) == true)
									Game_Map.print_Map(play_Character, Monster);
							}
						}
						else if (Character.Deck[card_num].MovementDown[j].Movement == "shield") {
							Character.Skill_shield(Character.Deck[card_num].MovementDown[j].Movement_Value);
							cout << Character.ID << " shield " << Character.Deck[card_num].MovementDown[j].Movement_Value << " this turn" << endl;
						}
						else if (Character.Deck[card_num].MovementDown[j].Movement == "heal") {
							Character.Skill_heal(Character.Deck[card_num].MovementDown[j].Movement_Value);
							cout << Character.ID << " heal " << Character.Deck[card_num].MovementDown[j].Movement_Value << ", now is " << Character.Hp << endl;
						}
						else if (Character.Deck[card_num].MovementDown[j].Movement == "move") {
							character_move(Character, Character.Deck[card_num].MovementDown[j].Movement_Value, Game_Map, play_Character, Monster);
							Game_Map.print_Map(play_Character, Monster);
						}
					}
					Character.Deck[card_num].status = 2;
					break;
				}
				else if(UpOrDown == 'd'&& temp.size() == 2)
				{
					for (int j = 0; j < Character.Deck[card_num].MovementDown.size(); j++)
					{
						if (Character.Deck[card_num].MovementDown[j].Movement == "attack") {
							if (Character.Deck[card_num].MovementDown[j].range == 0)
							{
								if (Character.Attack(Character.Deck[card_num].MovementDown[j].Movement_Value, Monster) == true)
									Game_Map.print_Map(play_Character, Monster);
							}
							else
							{
								if (Character.Attack(Character.Deck[card_num].MovementDown[j].Movement_Value, Character.Deck[card_num].MovementDown[j].range, Monster, Game_Map) == true)
									Game_Map.print_Map(play_Character, Monster);
							}
						}
						else if (Character.Deck[card_num].MovementDown[j].Movement == "shield") {
							Character.Skill_shield(Character.Deck[card_num].MovementDown[j].Movement_Value);
							cout << Character.ID << " shield " << Character.Deck[card_num].MovementDown[j].Movement_Value << " this turn" << endl;
						}
						else if (Character.Deck[card_num].MovementDown[j].Movement == "heal") {
							Character.Skill_heal(Character.Deck[card_num].MovementDown[j].Movement_Value);
							cout << Character.ID << " heal " << Character.Deck[card_num].MovementDown[j].Movement_Value << ", now is " << Character.Hp << endl;
						}
						else if (Character.Deck[card_num].MovementDown[j].Movement == "move") {
							character_move(Character, Character.Deck[card_num].MovementDown[j].Movement_Value, Game_Map, play_Character, Monster);
							Game_Map.print_Map(play_Character, Monster);
						}
					}
					Character.Deck[card_num].status = 2;
					for (int j = 0; j < Character.Deck.size(); j++) {
						if (Character.Deck[j].status == 4 && Character.Deck[j].ID != card_num) {
							card_num = Character.Deck[j].ID;
							break;
						}
					}
					for (int j = 0; j < Character.Deck[card_num].MovementUp.size(); j++)
					{
						if (Character.Deck[card_num].MovementUp[j].Movement == "attack") {
							if (Character.Deck[card_num].MovementUp[j].range == 0)
							{
								if (Character.Attack(Character.Deck[card_num].MovementUp[j].Movement_Value, Monster) == true)
									Game_Map.print_Map(play_Character, Monster);
							}
							else
							{
								if (Character.Attack(Character.Deck[card_num].MovementUp[j].Movement_Value, Character.Deck[card_num].MovementUp[j].range, Monster, Game_Map) == true)
									Game_Map.print_Map(play_Character, Monster);
							}
						}
						else if (Character.Deck[card_num].MovementUp[j].Movement == "shield") {
							Character.Skill_shield(Character.Deck[card_num].MovementUp[j].Movement_Value);
							cout << Character.ID << " shield " << Character.Deck[card_num].MovementUp[j].Movement_Value << " this turn" << endl;
						}
						else if (Character.Deck[card_num].MovementUp[j].Movement == "heal") {
							Character.Skill_heal(Character.Deck[card_num].MovementUp[j].Movement_Value);
							cout << Character.ID << " heal " << Character.Deck[card_num].MovementUp[j].Movement_Value << ", now is " << Character.Hp << endl;
						}
						else if (Character.Deck[card_num].MovementUp[j].Movement == "move") {
							character_move(Character, Character.Deck[card_num].MovementUp[j].Movement_Value, Game_Map, play_Character, Monster);
							Game_Map.print_Map(play_Character, Monster);
						}
					}
					Character.Deck[card_num].status = 2;
					break;
				}
				
			}
			else {
			cout << "Please Enter Right Command" << endl;
			}	
			cout << "請輸入動作指令:";
		}	
	}
	else
	{
		cout << Character.ID << "'turn: card -1" << endl;
		Character.long_Rest();
	}
}
void monsters_round(vector<Character>& play_Character, Ethnicity& Monster_Ethnicity, Monster_Base& monster, Map Game_Map,vector<Ethnicity>& Monster, vector<char> attack_Sort)
{
	if (Monster_Ethnicity.Command == -1) {
		return;
	}
	for (int i = 0; i < Monster_Ethnicity.Deck.size(); i++)
	{
		if (Monster_Ethnicity.Deck[i].status == 2) 
		{
			for (int j = 0; j < Monster_Ethnicity.Deck[i].Movement.size(); j++) 
			{
				if (Monster_Ethnicity.Deck[i].Movement[j].Movement == "attack") {
					if (monster_Attack(monster, Monster_Ethnicity.Deck[i].Movement[j].Movement_Value, Monster_Ethnicity.Deck[i].Movement[j].range, attack_Sort, Monster, play_Character, Game_Map) == true)
					{
						Game_Map.print_Map(play_Character, Monster);
					}
					//cout << "attack" << endl;
				}
				else if (Monster_Ethnicity.Deck[i].Movement[j].Movement == "shield") {
					monster.Skill_shield(Monster_Ethnicity.Deck[Monster_Ethnicity.Command].Movement[j].Movement_Value);
					cout << monster.icon << " shield " << Monster_Ethnicity.Deck[Monster_Ethnicity.Command].Movement[j].Movement_Value << " this turn" << endl;
				}
				else if (Monster_Ethnicity.Deck[i].Movement[j].Movement == "heal") {
					monster.Skill_heal(Monster_Ethnicity.Deck[Monster_Ethnicity.Command].Movement[j].Movement_Value);
					cout << monster.icon << " heal " << Monster_Ethnicity.Deck[Monster_Ethnicity.Command].Movement[j].Movement_Value << ", now is " << monster.Hp << endl;
				}
				else if (Monster_Ethnicity.Deck[i].Movement[j].Movement == "move") {
					monster_move(monster, Monster_Ethnicity.Deck[i].Movement[j].Move_Command, Game_Map, play_Character, Monster);
					Game_Map.print_Map(play_Character, Monster);
				}
			}
			break;
		}
	}
}
void end_round(vector<Character>& play_Character, vector<Ethnicity>& Monster, Map& Map) {
	bool no_monster = true;
	//護甲歸0
	for (int i = 0; i < play_Character.size(); i++) {
		play_Character[i].Shield = 0;
		play_Character[i].Rest = false;
	}
	for (int i = 0; i < Monster.size(); i++) {
		if (Monster[i].Command != -1) 
		{
			Monster[i].Deck[Monster[i].Command].status = 1;
		}
		for (int j = 0; j < Monster[i].Creature_List.size(); j++) {
			Monster[i].Creature_List[j].Shield = 0;
			if (Monster[i].Creature_List[j].active) {
				no_monster = false;
			}
		}
		//怪物判斷是否有重洗
		if (Monster[i].Shuffle_Mark) {
			for (int j = 0; j < Monster[i].Deck.size(); j++) {
				if (Monster[i].Deck[j].status == 1) {
					Monster[i].Deck[j].status = 0;
				}
			}
			Monster[i].Shuffle_Mark = false;
		}
	}
	//判斷角色有沒有在門上和判斷有沒有剩餘怪物 開門 and 重置角色長休狀態
	bool open_Door = false;
	for (int i = 0; i < play_Character.size(); i++) 
	{
		play_Character[i].Rest = false;
		if (Map.Game_Map[play_Character[i].position.y][play_Character[i].position.x] == 3 && no_monster == true) {
			Map.Game_Map[play_Character[i].position.y][play_Character[i].position.x] = 1;
			open_Door = true;
		}
	}
	//等所有門都開完再重新探視野
	
	if (open_Door == true) 
	{
		for (int i = 0; i < play_Character.size(); i++)
		{
			Map.check_road(play_Character[i].position.x, play_Character[i].position.y);
		}
		Map.print_Map(play_Character, Monster);
	}
	for (int i = 0; i < Monster.size(); i++)
	{
		for (int j = 0; j < Monster[i].Creature_List.size(); j++)
		{
			int x = Monster[i].Creature_List[j].position.x;
			int y = Monster[i].Creature_List[j].position.y;
			if (Map.Game_Map[y][x] == 1)
			{
				Monster[i].Creature_List[j].active = true;
			}
		}
	}
	/*for (int i = play_Character.size() - 1; i >= 0; i--)
	{
		int can_use_card = 0, can_throw_card = 0;
		for (int j = 0; j < play_Character[i].Deck.size(); j++) 
		{
			if (play_Character[i].Deck[j].status == 1)
				can_use_card++;
			if (play_Character[i].Deck[j].status == 2)
				can_throw_card++;
		}
		if (can_use_card < 2 && can_throw_card <2) 
		{
			play_Character.erase(play_Character.begin() + i);
			Map.print_Map(play_Character,Monster);
		}
	}*/
}
void character_move(Character& C, int step, Map& Game_Map, vector<Character> play_Character, vector<Ethnicity> Monster) {
	string position_input;
	Position start = C.position;
	bool wrong = false;
	do{
		wrong = false;
		C.position = start;
		cout << "請輸入移動指令:";
		std::cin >> position_input;
		if (position_input.size() <= step) 
		{
			for (int i = 0; i < position_input.size(); i++) 
			{
				switch (position_input[i])
				{
				case'w':
				case 'W':
					if (move_Error(C.position.x,C.position.y-1,play_Character,Monster,Game_Map)==true)
					{
						C.position.y--;
					}
					else
						wrong = true;
					break;
				case's':
				case 'S':
					if (move_Error(C.position.x, C.position.y+1, play_Character, Monster, Game_Map) == true)
					{
						C.position.y++;
					}
					else
						wrong = true;
					break;
				case'a':
				case 'A':
					if (move_Error(C.position.x-1, C.position.y, play_Character, Monster, Game_Map) == true)
					{
						C.position.x--;
					}
					else
						wrong = true;
					break;
				case'd':
				case 'D':
					if (move_Error(C.position.x+1, C.position.y, play_Character, Monster, Game_Map) == true)
					{
						C.position.x++;
					}
					else
						wrong = true;
					break;
				case'e':
				case 'E':
					break;
				}
				if (i == position_input.size() - 1) 
				{
					for (int j = 0; j < play_Character.size(); j++) 
					{
						if (play_Character[j].position.x == C.position.x && play_Character[j].position.y == C.position.y&&play_Character[j].ID!= C.ID)
						{
							wrong = true;
						}
					}
				}
				if (wrong) {
					cout << "Error Move!" << endl;
					break;
				}
			}
		}
		else {
			cout << "Error Move!" << endl;
			wrong = true;
		}
	} while (wrong == true);
}
//怪物移動
void monster_move(Monster_Base& C, string position_input, Map& Game_Map, vector<Character> play_Character, vector<Ethnicity> Monster) {
	Position start = C.position;
	vector<Position> way;
	for (int i = 0; i < position_input.size(); i++)
	{
		start = C.position;
		switch (position_input[i])
		{
		case'w':
		case 'W':
			if (move_Error_Monster(C.position.x, C.position.y - 1, play_Character, Game_Map) == true)
			{
				way.push_back(start);
				C.position.y--;
			}
			break;
		case's':
		case 'S':
			if (move_Error_Monster(C.position.x, C.position.y + 1, play_Character, Game_Map) == true)
			{
				way.push_back(start);
				C.position.y++;
			}
			break;
		case'a':
		case 'A':
			if (move_Error_Monster(C.position.x - 1, C.position.y, play_Character, Game_Map) == true)
			{
				way.push_back(start);
				C.position.x--;
			}
			break;
		case'd':
		case 'D':
			if (move_Error_Monster(C.position.x + 1, C.position.y, play_Character, Game_Map) == true)
			{
				way.push_back(start);
				C.position.x++;
			}
		}
		if (i == position_input.size() - 1)
		{
			bool check = true;
			int L = way.size() - 1;
			while (check) 
			{
				check = false;
				for (int j = 0; j < Monster.size(); j++)
				{
					for (int k = 0; k < Monster[j].Creature_List.size(); k++) {
						if (Monster[j].Creature_List[k].position.x == C.position.x && Monster[j].Creature_List[k].position.y == C.position.y&& Monster[j].Creature_List[k].icon!=C.icon)
						{
							C.position = way[L];
							L--;
							check = true;
						}
					}
				}
			}
			
		}
	}
	
}
bool vision_search(Position p1, Position p2, Map Map) {
	int xvalue, yvalue;
	if (p1.x > p2.x&& p1.y >= p2.y) {
		swap(p1, p2);
	}
	else if (p1.x > p2.x&& p1.y <= p2.y) {
		swap(p1, p2);
	}
	if (p1.x == p2.x) {
		xvalue = p1.x;
		if (p1.y > p2.y) {
			for (int i = p1.y; i >= p2.y; i--) {
				yvalue = i;
				if (Map.Game_Map[yvalue][xvalue] == 0) {
					return true;
				}
			}
		}
		else {
			for (int i = p1.y; i <= p2.y; i++) {
				yvalue = i;
				if (Map.Game_Map[yvalue][xvalue] == 0) {
					return true;
				}
			}
		}
	}
	//斜率為正 左上到右下
	else if (p1.x < p2.x && p2.y >= p1.y) {
		float tan = float(p1.y - p2.y) / float(p1.x - p2.x);
		//cout << tan << endl;
		for (float i = 0; i <= p2.x - p1.x; i += 0.0001) {
			xvalue = floor(i + p1.x + 0.5);
			yvalue = floor(i * tan + p1.y + 0.5);
			if (Map.Game_Map[yvalue][xvalue] == 0) {
				return true;
			}
		}
	}
	//斜率為負 左下到右上
	else if (p1.x < p2.x && p1.y >= p2.y) {
		float tan = float(p1.y - p2.y) / float(p1.x - p2.x) * -1;
		//cout << tan << endl;
		for (float i = 0; i <= p2.x - p1.x; i += 0.0001) {
			xvalue = floor(i + 0.5 + p1.x);
			yvalue = floor(p1.y + 0.5 - i * tan);
			if (Map.Game_Map[yvalue][xvalue] == 0) {
				return true;
			}
		}
	}
	return false;
}
bool monster_Attack(Monster_Base& M,int value,int range, vector<char> attack_Sort, vector<Ethnicity> Monster, vector<Character>& play_Character, Map& Game_Map)
{
	vector<int> target_List;
	int min_Distance = -1;
	for (int i = 0; i < play_Character.size(); i++)
	{
		if (min_Distance == -1)
			min_Distance = abs(M.position.x - play_Character[i].position.x) + abs(M.position.y - play_Character[i].position.y);
		int distance = abs(M.position.x - play_Character[i].position.x) + abs(M.position.y - play_Character[i].position.y);
		if (range + M.Range == 0)
		{
			if (distance == 1)
			{
				target_List.push_back(i);
			}
		}
		else
		{
			if (distance <= range + M.Range && vision_search(M.position, play_Character[i].position, Game_Map) == false)
			{
				target_List.push_back(i);
			}
		}
		if (distance < min_Distance)
			min_Distance = distance;
	}
	for (int i = target_List.size() - 1; i >= 0; i--)
	{
		if ((abs(M.position.x - play_Character[target_List[i]].position.x) + abs(M.position.y - play_Character[target_List[i]].position.y)) > min_Distance)
			target_List.erase(target_List.begin() + i);
	}
	int final_Target = 99;
	for (int i = 0; i < attack_Sort.size(); i++)
	{
		for (int j = 0; j < target_List.size(); j++)
		{
			if (play_Character[target_List[j]].ID == attack_Sort[i])
			{
				final_Target = target_List[j];
			}
		}
		if (final_Target != 99)
			break;
	}
	if (final_Target == 99)
	{
		cout << "no one lock" << endl;
		return false;
	}
	int distance = abs(M.position.x - play_Character[final_Target].position.x) + abs(M.position.y - play_Character[final_Target].position.y);
	cout << M.icon << " lock " << play_Character[final_Target].ID << " in distance " << distance << endl;
	//b attack A 3 damage, A shield 1, A remain 12 hp
	cout << M.icon << " attack " << play_Character[final_Target].ID << " " << (M.Damage + value) << " damage, " << play_Character[final_Target].ID << " shield " << play_Character[final_Target].Shield << ", " << play_Character[final_Target].ID << " remain ";
	if (play_Character[final_Target].Shield < M.Damage + value)
	{
		play_Character[final_Target].Hp -= ((M.Damage + value) - play_Character[final_Target].Shield);
	}
	else
	{
		
	}
	cout << play_Character[final_Target].Hp << " hp" << endl;
	if (play_Character[final_Target].Hp <= 0)
	{
		cout << play_Character[final_Target].ID << " is killed!!" << endl;
		play_Character.erase(play_Character.begin() + final_Target);
	}
	return false;
}
//計算角色棄牌堆的數量
int calculate_Discard(Character C) {
	int num = 0;
	for (int i = 0; i < C.Deck.size(); i++) {
		if (C.Deck[i].status == 2) {
			num++;
		}
	}
	return num;
}
//判斷是否打手牌
bool IsPlayHandCard(Character &C,string command ) {
	if (command.size() > 1) {
		return false;
	}
	else if (command.size() == 1 && command[0] >= 48 && command[0] <= 57) {
		for (int i = 0; i < C.Deck.size(); i++) {
			if (C.Deck[i].ID == (command[0] - '0')&&C.Deck[i].status==1) {
				C.Deck[i].status = 4;
				return true;
			}
		}
		return false;
	}
	else {
		return false;
	}
}
//判斷是否為手牌 若是的話狀態改為出牌
bool IsCardInHand(Character &C,int card) {
	for (int i = 0; i < C.Deck.size(); i++) {
		if (C.Deck[i].ID == card && C.Deck[i].status == 1) {
			C.Deck[i].status = 4;
			return true;
		}
	}
	return false;
}
//Check
void check(Character C) {
	cout << "HandCard:";
	for (int i = 0; i < C.Deck.size(); i++) {
		if (C.Deck[i].status == 1||C.Deck[i].status==4) {
			cout << C.Deck[i].ID << " ";
		}
	}
	cout << ";";
	cout << "Discard:";
	for (int i = 0; i < C.Deck.size(); i++) {
		if (C.Deck[i].status == 2) {
			cout << C.Deck[i].ID << " ";
		}
	}
	cout << endl;
}
bool check_player_done(bool* player, int num) {
	for (int i = 0; i < num; i++) {
		if (!player[i]) {
			return true;
		}
	}
	return false;
}
//列出攻擊順序
void show_AttackList(vector<char> attack_Sort, vector<Character> Play_Character, vector<Ethnicity> Monster)
{
	for (int i = 0; i < attack_Sort.size(); i++) 
	{
		if (attack_Sort[i] <= 'Z' && attack_Sort[i] >= 'A') 
		{
			for (int j = 0; j < Play_Character.size(); j++) 
			{
				if (Play_Character[j].ID == attack_Sort[i]) 
				{
					if (Play_Character[j].Command[0] != -1) 
					{
						cout<<"角色:" << Play_Character[j].ID << " " << setw(2) << setfill('0')<<"敏捷值:"<< Play_Character[j].Deck[Play_Character[j].Command[0]].Dexterity_Value << " 卡片一:" << Play_Character[j].Command[0] << " 卡片二:" << Play_Character[j].Command[1] << endl;
					}
					else 
					{
						cout << Play_Character[j].ID << " 99 -1" << endl;
					}
				}
			}
		}
		else 
		{
			for (int j = 0; j < Monster.size(); j++) 
			{
				if (Monster[j].Command == -1)
					continue;
				cout << Monster[j].Ethnicity_Name << " " << setw(2) << setfill('0') << Monster[j].Deck[Monster[j].Command].Dexterity_Value<<" ";
				for (int L = 0; L < Monster[j].Deck[Monster[j].Command].Movement.size(); L++) 
				{
					cout << Monster[j].Deck[Monster[j].Command].Movement[L].Movement << " ";
					if (Monster[j].Deck[Monster[j].Command].Movement[L].Movement == "attack") 
					{
						cout << Monster[j].Deck[Monster[j].Command].Movement[L].Movement_Value << " " << "range " << Monster[j].Deck[Monster[j].Command].Movement[L].range << " ";
					}
					else if (Monster[j].Deck[Monster[j].Command].Movement[L].Movement == "move") 
					{
						cout << Monster[j].Deck[Monster[j].Command].Movement[L].Move_Command << " ";
					}
					else
					{
						cout << Monster[j].Deck[Monster[j].Command].Movement[L].Movement_Value << " ";
					}
				}
				cout << endl;
					
				
			}
		}
	}
}
//取得角色敏捷值by name
int get_Character_Dex(vector<Character> Play_Character,char name, int num) 
{
	for (int i = 0; i < Play_Character.size(); i++) 
	{
		if (Play_Character[i].ID == name) 
		{
			if (Play_Character[i].Command[0] == -1)
				return 99;
			else
				return Play_Character[i].Deck[Play_Character[i].Command[num]].Dexterity_Value;
		}
	}
}
int get_Monster_Dex(vector<Ethnicity> Monster, char name)
{
	for (int i = 0; i < Monster.size(); i++) 
	{
		for (int j = 0; j < Monster[i].Creature_List.size(); j++)
		{
			if (Monster[i].Creature_List[j].icon == name)
				return Monster[i].Deck[Monster[i].Command].Dexterity_Value;
		}
	}
}
//讀檔
void read_Character_Data(fstream& File_Charactervector, vector<Character>& Base_Character)
{
	int Charactor_count,Hp,Hand,Deck; string Charactor_Name;
	/*========讀取幾個角色模板=========*/
	File_Charactervector >> Charactor_count;
	for (int i = 0; i < Charactor_count; i++) 
	{
		/*========讀取角色模板資訊=========*/
		File_Charactervector >> Charactor_Name >> Hp >> Hand >> Deck;
		Character Base_type(Charactor_Name,Hp,Hand);		//新的模板
		/*====讀取角色模板卡片資訊====*/
		for (int j = 0; j < Deck; j++)	//一組共有Deck張卡 
		{
			Player_Card newCard;	//新的卡片
			string per_line,upString,downString;string skill_Name;	int skill_Value;
			File_Charactervector >> newCard.ID >> newCard.Dexterity_Value;		//讀取卡片的ID 和 敏捷值
			getline(File_Charactervector, per_line);		//截下整段能力值
			stringstream interval(per_line);
			getline(interval, upString, '-');getline(interval, downString);		//以"-"為界將整段字串分割成 上部分的技能 和 下部分的技能
			interval.clear(); interval.str("");		//清空stringstream
			interval << upString;
			while (interval >> skill_Name >> skill_Value)	//用"空格"將字串分割成 能力 和 值	
			{
				if (skill_Name == "range") 
				{
					newCard.MovementUp[newCard.MovementUp.size() - 1].range = skill_Value;
				}
				else 
				{
					Card_Property newMovement;
					newMovement.Movement = skill_Name, newMovement.Movement_Value = skill_Value;
					newCard.MovementUp.push_back(newMovement);
				}
			}
			interval.clear(); interval.str("");		//清空stringstream
			interval << downString;
			while (interval >> skill_Name >> skill_Value)	//用"空格"將字串分割成 能力 和 值	
			{
				if (skill_Name == "range")
				{
					newCard.MovementDown[newCard.MovementDown.size() - 1].range = skill_Value;
				}
				else
				{
					Card_Property newMovement;
					newMovement.Movement = skill_Name, newMovement.Movement_Value = skill_Value;
					newCard.MovementDown.push_back(newMovement);
				}
			}
			interval.clear(); interval.str("");
			Base_type.Deck.push_back(newCard);		//將新的卡片加進卡組
		}
		Base_Character.push_back(Base_type);		//將新的角色模板加入模板列表
	}
}
void read_Monster_Data(fstream& File_Monster, vector<Ethnicity>& Base_Monster) 
{
	int Monster_count;
	File_Monster >> Monster_count;		//種族數量
	for (int i = 0; i < Monster_count; i++) 
	{
		string Ethnicity_Name; int hp, atk, range, elite_hp, elite_damage, elite_range;
		File_Monster >> Ethnicity_Name >> hp >> atk >> range >> elite_hp >> elite_damage >> elite_range;
		Ethnicity Base_Type(Ethnicity_Name, hp, atk,range, elite_hp, elite_damage, elite_range);	//宣告怪物模板
		for (int j = 0; j < 6; j++)		//怪物固定六張技能卡
		{
			string per_line, Movement;
			int skill_Value;
			stringstream interval("");
			Creature_Card newCard;	//新的卡片
			File_Monster >> Ethnicity_Name >> newCard.ID >> newCard.Dexterity_Value;	//讀取卡片資料
			getline(File_Monster, per_line);	//截下整段能力值
			interval << per_line;
			while (interval >> Movement)	//以"空格"分割字串
			{
				Card_Property newMovement;
				if (Movement == "move")		//判斷是否為"move指令"
				{
					interval >> newMovement.Move_Command;
				}
				else if (Movement == "d")	//如果是重洗標誌就跳出
				{
					newCard.Shuffle_Mark = false;
					break;
				}
				else if (Movement == "r")	//如果是重洗標誌就跳出
				{
					newCard.Shuffle_Mark = true; 
					break;
				}
				else
				{
					interval >> skill_Value;
					if (Movement == "range") 
					{
						newCard.Movement[newCard.Movement.size() - 1].range = skill_Value;
						continue;
					}
					else 
					{
						newMovement.Movement_Value = skill_Value;
					}
				}
				newMovement.Movement = Movement;
				newCard.Movement.push_back(newMovement);
			}
			interval.clear();		//清空stringstream
			interval.str("");
			Base_Type.Deck.push_back(newCard);
		}
		Base_Monster.push_back(Base_Type);
	}
}
void read_Map_Data(fstream& File_Map, Map& Map, vector<Ethnicity>& Monster,int Player_num)
{
	string File_Name;	
	do 
	{
		cout << "請輸入地圖位址：";
		cin >> File_Name;	//輸入地圖檔名
		File_Map.open(File_Name, ios::in);	//正式用
		if (!File_Map)
			cout << "找不到位址檔案" << endl;
	} while (!File_Map);
	//File_Map.open("map1.txt", ios::in);		//測試用
	File_Map >> Map.High >> Map.Width;
	vector<int>temp;
	for (int i = 0; i < Map.High; i++)
	{
		for (int j = 0; j < Map.Width; j++)
		{
			char temp_num;
			File_Map >> temp_num;
			int temp_num_int = temp_num - '0';
			if (temp_num_int == 1)
			{
				temp_num_int = 4;
			}
			else if (temp_num_int == 2)
			{
				temp_num_int = 6;
			}
			temp.push_back(temp_num_int);
		}
		Map.Game_Map.push_back(temp);
		temp.clear();
	}
	/*for (int i = 0; i < Map.High; i++) 
	{
		for (int j = 0; j < Map.Width; j++)
		{
			cout << Map.Game_Map[i][j];
		}
		cout << endl;
	}*/
	int x, y;
	for (int i = 0; i < 4; i++) 
	{
		File_Map >> x >> y;
		Position temp;
		temp.x = x;
		temp.y = y;
		Map.Game_Map[y][x] = 5;
		Map.Init_Pos.push_back(temp);
	}
	File_Map >> Map.Monster_Count;
	int a = Monster.size();
	for (int i = 0; i < Map.Monster_Count; i++) 
	{
		string Monster_Type;
		Monster_Base Monster_temp;
		Position temp;
		int Status[3];
		int mode=0;
		File_Map >> Monster_Type;
		for (int j = 0; j < Monster.size(); j++) 
		{
			if (Monster_Type == Monster[j].Ethnicity_Base_value.Name) 
			{
				File_Map >> temp.x >> temp.y >> Status[0] >> Status[1] >> Status[2];
				mode = Status[Player_num - 2];
			}
			else 
				continue;
			if (mode == 0) 
			{
				break;
			}
			else if (mode == 1) 
			{
				Monster_temp.Damage = Monster[j].Ethnicity_Base_value.Damage;
				Monster_temp.Hp = Monster[j].Ethnicity_Base_value.Hp;
				Monster_temp.Max_HP = Monster[j].Ethnicity_Base_value.Hp;
				Monster_temp.Name = Monster[j].Ethnicity_Base_value.Name;
				Monster_temp.Range = Monster[j].Ethnicity_Base_value.Range;
				Monster_temp.mode = mode;
				Monster_temp.position = temp;
				Monster_temp.icon = 'a' + i;
				Monster[j].Creature_List.push_back(Monster_temp);
			}
			else 
			{
				Monster_temp.Damage = Monster[j].Ethnicity_Base_value.Elite_Damage;
				Monster_temp.Hp = Monster[j].Ethnicity_Base_value.Elite_Hp;
				Monster_temp.Max_HP = Monster[j].Ethnicity_Base_value.Elite_Hp;
				Monster_temp.Name = Monster[j].Ethnicity_Base_value.Name;
				Monster_temp.Range = Monster[j].Ethnicity_Base_value.Elite_Range;
				Monster_temp.mode = mode;
				Monster_temp.position = temp;
				Monster_temp.icon = 'a' + i;
				Monster[j].Creature_List.push_back(Monster_temp);
			}
		}	
	}
	File_Map.close();
}
//確認怪物狀態
void check_Monsters_Active(vector<Ethnicity>& Monster, Map Game_Map) 
{
	for (int i = 0; i < Monster.size(); i++) 
	{
		for (int j = 0; j < Monster[i].Creature_List.size(); j++) 
		{
			int x = Monster[i].Creature_List[j].position.x; int y = Monster[i].Creature_List[j].position.y;
			if (Game_Map.Game_Map[y][x] == 1) 
			{
				Monster[i].Creature_List[j].active = true;
			}
		}
	}
}
bool find_by_step(int x1, int y1, int x2, int y2, int step)
{
	if (x1 == x2 && y1 == y2)
	{
		return true;
	}
	if (step > 0)
	{
		if (find_by_step(x1 + 1, y1, x2, y2, step - 1) == true
			|| find_by_step(x1, y1 + 1, x2, y2, step - 1) == true
			|| find_by_step(x1 - 1, y1, x2, y2, step - 1) == true
			|| find_by_step(x1, y1 - 1, x2, y2, step - 1) == true)
		{
			return true;
		}
	}
	else if (step == 0)
	{
		return false;
	}
}
bool find_by_step_2(int x1, int y1, int x2, int y2, int step, int& distance, Map game_map)
{
	if (x1 == x2 && y1 == y2)
	{
		distance = step;
		return true;
	}
	if (step > 0 && game_map.Game_Map[y1][x1] == 1)
	{
		if (find_by_step(x1 + 1, y1, x2, y2, step - 1) == true
			|| find_by_step(x1, y1 + 1, x2, y2, step - 1) == true
			|| find_by_step(x1 - 1, y1, x2, y2, step - 1) == true
			|| find_by_step(x1, y1 - 1, x2, y2, step - 1) == true)
		{
			return true;
		}
	}
	else if (step == 0)
	{
		return false;
	}
}
//確認移動路徑
bool move_Error(int x,int y,vector<Character> play_Character, vector<Ethnicity> Monster, Map Game_Map)
{
	if (Game_Map.Game_Map[y][x] != 1 && Game_Map.Game_Map[y][x] != 3)
	{
		return false;
	}
	for (int i = 0; i < Monster.size(); i++) 
	{
		for (int j = 0; j < Monster[i].Creature_List.size(); j++)
		{
			if (Monster[i].Creature_List[j].position.x == x && Monster[i].Creature_List[j].position.y == y) 
			{
				return false;
			}
		}
	}
	return true;
}
bool move_Error_Monster(int x, int y, vector<Character> play_Character, Map Game_Map)
{
	if (Game_Map.Game_Map[y][x] != 1)
	{
		return false;
	}
	for (int i = 0; i < play_Character.size(); i++)
	{
		if (play_Character[i].position.x == x && play_Character[i].position.y == y)
		{
			return false;
		}
	}
	return true;
}
//
void print_character_sort_onActive_monster(vector<Ethnicity> Monster, vector<Character> play_Character) {
	vector<Monster_Base>temp;
	for (int i = 0; i < play_Character.size(); i++) {
		cout << play_Character[i].ID << "-hp:" << play_Character[i].Hp << ", shield: " << play_Character[i].Shield << endl;
	}
	for (int i = 0; i < Monster.size(); i++)
	{
		for (int j = 0; j < Monster[i].Creature_List.size(); j++)
		{
			if (Monster[i].Creature_List[j].active)
			{
				temp.push_back(Monster[i].Creature_List[j]);
			}
		}
	}
	sort(temp.begin(), temp.end(),abcSort);
	for (int i = 0; i < temp.size(); i++) {
		cout << temp[i].icon << "-hp:" << temp[i].Hp << ", shield: " << temp[i].Shield << endl;
	}
}
bool abcSort(Monster_Base a,Monster_Base b) {
	if (a.icon > b.icon)
	{
		return false;
	}
	else {
		return true;
	}
}
/*==============DEBUG_MODE================*/

void get_All_Base_Character_Data(vector<Character> Base_Player) 
{
	for (int i = 0; i < Base_Player.size(); i++)
	{
		cout << "角色名：" << Base_Player[i].Character_name << "  角色 HP：" << Base_Player[i].Hp << " 角色手牌數：" << Base_Player[i].Hand << endl << "技能卡：" << endl;
		for (int j = 0; j < Base_Player[i].Deck.size(); j++)
		{
			cout << "ID：	" << Base_Player[i].Deck[j].ID << " 狀態: " << Base_Player[i].Deck[j].status << "	敏捷值：	" << Base_Player[i].Deck[j].Dexterity_Value << "	上：";
			for (int k = 0; k < Base_Player[i].Deck[j].MovementUp.size(); k++) 
			{
				cout << Base_Player[i].Deck[j].MovementUp[k].Movement << " " << Base_Player[i].Deck[j].MovementUp[k].Movement_Value << " ";
				if (Base_Player[i].Deck[j].MovementUp[k].Movement == "attack")
					cout << "range " << Base_Player[i].Deck[j].MovementUp[k].range << " ";
			}
			cout << " | 下：";
			for (int k = 0; k < Base_Player[i].Deck[j].MovementDown.size(); k++) 
			{
				cout << Base_Player[i].Deck[j].MovementDown[k].Movement << " " << Base_Player[i].Deck[j].MovementDown[k].Movement_Value << " ";
				if (Base_Player[i].Deck[j].MovementDown[k].Movement == "attack")
					cout <<"range "<< Base_Player[i].Deck[j].MovementDown[k].range << " ";
			}
			cout << endl;
		}
		cout << endl << endl;
	}
}
void get_All_Base_Monster_Data(vector<Ethnicity> Base_Monster) 
{
	for (int i = 0; i < Base_Monster.size(); i++) 
	{
		cout << "種族名：" << Base_Monster[i].Ethnicity_Name
			<< " 普通生命：" << Base_Monster[i].Ethnicity_Base_value.Hp
			<< " 普通攻擊：" << Base_Monster[i].Ethnicity_Base_value.Damage
			<< " 普通範圍：" << Base_Monster[i].Ethnicity_Base_value.Range
			<< " 菁英生命：" << Base_Monster[i].Ethnicity_Base_value.Elite_Hp
			<< " 菁英攻擊：" << Base_Monster[i].Ethnicity_Base_value.Elite_Damage
			<< " 菁英範圍：" << Base_Monster[i].Ethnicity_Base_value.Elite_Range << endl << endl;
		for (int j = 0; j < Base_Monster[i].Creature_List.size(); j++)
		{
			cout << "怪物" << Base_Monster[i].Creature_List[j].icon;
			if (Base_Monster[i].Creature_List[j].mode == 2)
				cout << ": 菁英";
			else
				cout << ": 普通";
			cout << " 生命值:" << Base_Monster[i].Creature_List[j].Hp
				<< " 傷害:" << Base_Monster[i].Creature_List[j].Damage
				<< " 範圍:" << Base_Monster[i].Creature_List[j].Range << endl;
		}
		cout << "技能卡：" << endl;
		for (int j = 0; j < 6; j++) 
		{
			cout << "ID：" << Base_Monster[i].Deck[j].ID << " 狀態: " << Base_Monster[i].Deck[j].status << "	敏捷值：" << Base_Monster[i].Deck[j].Dexterity_Value << "	";
			for (int k = 0; k < Base_Monster[i].Deck[j].Movement.size(); k++) 
			{
				cout << Base_Monster[i].Deck[j].Movement[k].Movement<<" ";
				if (Base_Monster[i].Deck[j].Movement[k].Movement == "move") 
				{
					cout << Base_Monster[i].Deck[j].Movement[k].Move_Command << " ";
				}
				else 
				{
					cout << Base_Monster[i].Deck[j].Movement[k].Movement_Value << " ";
					if (Base_Monster[i].Deck[j].Movement[k].Movement == "attack")
						cout << " range " << Base_Monster[i].Deck[j].Movement[k].range << " ";
				}
			}
			cout <<"重洗標誌："<< Base_Monster[i].Deck[j].Shuffle_Mark << endl;
		}
		cout << endl << endl;
	}
}
void get_int_Map(Map Map) 
{
	for (int i = 0; i < Map.High; i++) {
		for (int j = 0; j < Map.Width; j++) {
			cout << Map.Game_Map[i][j];
		}
		cout << endl;
	}
}
void check_all_dex(vector<Character> Play_Character, vector<Ethnicity> Monster) 
{
	cout << endl << "player:" << endl;
	for (int i = 0; i < Play_Character.size(); i++) 
	{
		cout << "	" << Play_Character[i].ID << " : " << Play_Character[i].Deck[Play_Character[i].Command[0]].Dexterity_Value << " " << Play_Character[i].Deck[Play_Character[i].Command[1]].Dexterity_Value << endl;
	}
	cout << "monsters:" << endl;
	for (int i = 0; i <Monster.size(); i++)
	{
		if (Monster[i].Command == -1) 
		{
			cout << "	" << Monster[i].Ethnicity_Name << " : deactivate." << endl;
		}
		else 
		{
			cout << "	" << Monster[i].Ethnicity_Name << " : " << Monster[i].Deck[Monster[i].Command].Dexterity_Value << endl;
		}
	}
}

/*================UI_MODE=================*/

void Main_Game_UI(fstream& File_Character, fstream& File_Monster, fstream& File_Map)
{
	vector<Character> Base_Character;	//角色模板，，用於之後創建角色清單時從裡面複製角色資料
	vector<Ethnicity> Monster;			//所有種族
	vector<Character> play_Character;	//玩家選擇的角色列表
	read_Character_Data(File_Character, Base_Character);	//Character讀檔
	read_Monster_Data(File_Monster, Monster);			//Monster讀檔

	while (1) 
	{
		system("cls");
		if (Project_Start_UI() == 1)
		{
			system("cls");
			cout << "感謝您的遊玩" << endl << endl;
			system("pause");
			return;
		}
		system("cls");
		Map GameMap; //所有Map
		creat_Character_UI(Base_Character,play_Character);
		read_Map_Data(File_Map, GameMap, Monster, play_Character.size()); //Map讀檔
		//get_All_Base_Character_Data(play_Character);	//檢查Character資料
		//get_All_Base_Monster_Data(Monster);				//檢查Monster資料
		choose_Start_Position_UI(play_Character, Monster, GameMap);		//選擇起始位置
		check_Monsters_Active(Monster, GameMap);	//檢查怪物狀態
		main_Battle_UI(play_Character, Monster, GameMap);
		system("pause");
		//重置
		play_Character.clear();
		for (int i = 0; i < Monster.size(); i++)
		{
			Monster[i].Creature_List.clear();
			Monster[i].Shuffle_Mark = false;
			for (int j = 0; j < Monster[i].Deck.size(); j++)
				Monster[i].Deck[j].status = 0;
		}
		GameMap.~Map();
	}

	

}

//開始介面  回傳值 = 0:開始遊戲 ，回傳值 = 1:結束遊戲
int Project_Start_UI() 
{
	cout << "    GloomHaven		by B10815044謝鈞曜、B10815058卜啟皓" << endl << endl;
	cout << "=================" << endl;
	cout << "=      "; SetColor(240); cout << "開始"; SetColor(7);cout << "     =" << endl;
	cout << "=      離開     =" << endl;
	cout << "=================" << endl << endl << endl;
	int mode = 0;			//選擇要開始遊戲或是離開
	char keyBoard_Input;	//鍵盤輸入
	setPrintPosition(0, 49);
	while (1) 
	{
		keyBoard_Input = _getch();
		//將反白的文字用原來的顏色蓋過
		if (mode == 1)
		{
			setPrintPosition(7, 4);
			SetColor(7); cout << "離開";
		}
		else if (mode == 0)
		{
			setPrintPosition(7, 3);
			SetColor(7); cout << "開始";
		}
		//根據keyBoard輸入決定要哪一個選項
		switch (keyBoard_Input)
		{
		case 'W':
		case 'w':
		case 72:
			if (mode == 0) 
				mode = 1;
			else 
				mode--;
			break;
		case 'S':
		case 's':
		case 80:
			if (mode == 1)
				mode = 0;
			else
				mode++;
			break;
		}
		//將原本的文字用反白蓋過
		if (mode == 1)
		{
			setPrintPosition(7, 4);
			SetColor(240); cout << "離開";
		}
		else if (mode == 0)
		{
			setPrintPosition(7, 3);
			SetColor(240); cout << "開始";
		}
		setPrintPosition(0, 49);	SetColor(7);	//重置 print點 和 顏色 (若要使用setPrintPosition 或是 SetColor 使用後，請使用這段重置他們的設定)
		if (keyBoard_Input == 13)		//Enter鍵 回傳mode;
			return mode;
	}
}
void creat_Character_UI(vector<Character>& Base_Character, vector<Character>& play_Character)
{
	cout << "======請選擇角色數量======" << endl;
	cout << "==■■■  ■■■  ■  ■==" << endl;
	cout << "==    ■      ■  ■  ■==" << endl;
	cout << "==■■■  ■■■  ■■■==" << endl;
	cout << "==■          ■      ■==" << endl;
	cout << "==■■■  ■■■      ■==" << endl;
	cout << "==========================" << endl;
	int mode = 3;
	int nowMode = 2;
	bool chooseComplete = false;
	while (chooseComplete == false) 
	{
		if (nowMode != mode) 
		{
			SetColor(7);
			switch (mode)
			{
				//(2,1)(10,1)(18,1)
				//消掉
			case 2:
				setPrintPosition(2, 1); cout << "■■■";
				setPrintPosition(2, 2); cout << "    ■";
				setPrintPosition(2, 3); cout << "■■■";
				setPrintPosition(2, 4); cout << "■    ";
				setPrintPosition(2, 5); cout << "■■■";
				break;
			case 3:
				setPrintPosition(10, 1); cout << "■■■";
				setPrintPosition(10, 2); cout << "    ■";
				setPrintPosition(10, 3); cout << "■■■";
				setPrintPosition(10, 4); cout << "    ■";
				setPrintPosition(10, 5); cout << "■■■";
				break;
			case 4:
				setPrintPosition(18, 1); cout << "■  ■";
				setPrintPosition(18, 2); cout << "■  ■";
				setPrintPosition(18, 3); cout << "■■■";
				setPrintPosition(18, 4); cout << "    ■";
				setPrintPosition(18, 5); cout << "    ■";
				break;
			}
			setPrintPosition(0, 0);
			SetColor(14);
			switch (nowMode) 
			{
				//塗色
			case 2:
				setPrintPosition(2, 1); cout << "■■■";
				setPrintPosition(2, 2); cout << "    ■";
				setPrintPosition(2, 3); cout << "■■■";
				setPrintPosition(2, 4); cout << "■    ";
				setPrintPosition(2, 5); cout << "■■■";
				break;
			case 3:
				setPrintPosition(10, 1); cout << "■■■";
				setPrintPosition(10, 2); cout << "    ■";
				setPrintPosition(10, 3); cout << "■■■";
				setPrintPosition(10, 4); cout << "    ■";
				setPrintPosition(10, 5); cout << "■■■";
				break;
			case 4:
				setPrintPosition(18, 1); cout << "■  ■";
				setPrintPosition(18, 2); cout << "■  ■";
				setPrintPosition(18, 3); cout << "■■■";
				setPrintPosition(18, 4); cout << "    ■";
				setPrintPosition(18, 5); cout << "    ■";
				break;
			}
			mode = nowMode;
			SetColor(7);
			setPrintPosition(0, 49);
		}
		else 
		{
			switch (keyBoard(_getch()))
			{
			case 'a':
				nowMode--;
				if (nowMode == 1)
					nowMode = 4;
				break;
			case 'd':
				nowMode++;
				if (nowMode == 5)
					nowMode = 2;
				break;
			case 13:
				system("cls");
				//跳出
				chooseComplete = true;
				break;
			}
		}
	}
	for (int i = 0; i < mode; i++) 
	{
		char output = 'A' + i;
		cout << "========請選擇角色" << output << "=======" << endl << endl;
		int j, maxNameLength = 0;
		for (j = 0; j < Base_Character.size(); j++) 
		{
			if (maxNameLength < Base_Character[j].Character_name.size())
				maxNameLength = Base_Character[j].Character_name.size();
		}
		for (j = 0; j < Base_Character.size(); j++) 
		{
			setPrintPosition(0, 2 + j); cout << "=   " << Base_Character[j].Character_name; setPrintPosition(6 + maxNameLength, 2 + j); cout << "= ";
		}
		chooseComplete = false;
		int character = 0, nowCharacter = 0, finalCharacter = j - 1;
		setPrintPosition(0, finalCharacter + 4);
		cout << "========角色資訊========";
		setPrintPosition(1, character + 2); cout << "  ";
		setPrintPosition(1, nowCharacter + 2); cout << "→";
		setPrintPosition(0, finalCharacter + 6);
		for (j = 0; j < 6 + Base_Character[character].Deck.size(); j++)
		{
			cout << "                                                                                                                         " << endl;
		}
		setPrintPosition(0, finalCharacter + 6);
		cout << "角色名稱: " << Base_Character[nowCharacter].Character_name << endl;
		cout << "角色血量: " << Base_Character[nowCharacter].Hp << endl;
		cout << "起始手排數量: " << Base_Character[nowCharacter].Hand << endl << endl;
		cout << "=====角色可選擇卡牌=====" << endl;
		for (j = 0; j < Base_Character[nowCharacter].Deck.size(); j++)
		{
			cout << "編號：	" << Base_Character[nowCharacter].Deck[j].ID << "	敏捷值：" << Base_Character[nowCharacter].Deck[j].Dexterity_Value << "	上：";
			for (int k = 0; k < Base_Character[nowCharacter].Deck[j].MovementUp.size(); k++)
			{
				cout << Base_Character[nowCharacter].Deck[j].MovementUp[k].Movement << " " << Base_Character[nowCharacter].Deck[j].MovementUp[k].Movement_Value << " ";
				if (Base_Character[nowCharacter].Deck[j].MovementUp[k].Movement == "attack")
					cout << "range " << Base_Character[nowCharacter].Deck[j].MovementUp[k].range << " ";
			}
			cout << " | 下：";
			for (int k = 0; k < Base_Character[nowCharacter].Deck[j].MovementDown.size(); k++)
			{
				cout << Base_Character[nowCharacter].Deck[j].MovementDown[k].Movement << " " << Base_Character[nowCharacter].Deck[j].MovementDown[k].Movement_Value << " ";
				if (Base_Character[nowCharacter].Deck[j].MovementDown[k].Movement == "attack")
					cout << "range " << Base_Character[nowCharacter].Deck[j].MovementDown[k].range << " ";
			}
			cout << endl;
		}
		character = nowCharacter;
		setPrintPosition(0, 49);
		while (chooseComplete == false) 
		{
			if (character != nowCharacter) 
			{
				setPrintPosition(1, character + 2); cout << "  ";
				setPrintPosition(1, nowCharacter + 2); cout << "→";
				setPrintPosition(0, finalCharacter + 6);
				for (j = 0; j < 6 + Base_Character[character].Deck.size(); j++) 
				{
					cout << "                                                                                                                         " << endl;
				}
				setPrintPosition(0, finalCharacter + 6);
				cout << "角色名稱: " << Base_Character[nowCharacter].Character_name << endl;
				cout << "角色血量: " << Base_Character[nowCharacter].Hp << endl;
				cout << "起始手排數量: " << Base_Character[nowCharacter].Hand << endl << endl;
				cout << "=====角色可選擇卡牌=====" << endl;
				for (j = 0; j < Base_Character[nowCharacter].Deck.size(); j++)
				{
					cout << "編號：	" << Base_Character[nowCharacter].Deck[j].ID << "	敏捷值：" << Base_Character[nowCharacter].Deck[j].Dexterity_Value << "	上：";
					for (int k = 0; k < Base_Character[nowCharacter].Deck[j].MovementUp.size(); k++)
					{
						cout << Base_Character[nowCharacter].Deck[j].MovementUp[k].Movement << " " << Base_Character[nowCharacter].Deck[j].MovementUp[k].Movement_Value << " ";
						if (Base_Character[nowCharacter].Deck[j].MovementUp[k].Movement == "attack")
							cout << "range " << Base_Character[nowCharacter].Deck[j].MovementUp[k].range << " ";
					}
					cout << " | 下：";
					for (int k = 0; k < Base_Character[nowCharacter].Deck[j].MovementDown.size(); k++)
					{
						cout << Base_Character[nowCharacter].Deck[j].MovementDown[k].Movement << " " << Base_Character[nowCharacter].Deck[j].MovementDown[k].Movement_Value << " ";
						if (Base_Character[nowCharacter].Deck[j].MovementDown[k].Movement == "attack")
							cout << "range " << Base_Character[nowCharacter].Deck[j].MovementDown[k].range << " ";
					}
					cout << endl;
				}
				character = nowCharacter;
				setPrintPosition(0, 49);
			}
			else 
			{
				switch (keyBoard(_getch()))
				{
				case 'w':
					nowCharacter--;
					if (nowCharacter < 0)
						nowCharacter = finalCharacter;
					break;
				case 's':
					nowCharacter++;
					if (nowCharacter > finalCharacter)
						nowCharacter = 0;
					break;
				case 13:
					chooseComplete = true;
					break;
				}
			}
		}
		setPrintPosition(0, finalCharacter + 10);
		chooseComplete = false;
		vector<int> chooseCard;
		int nowCard = 0, card = 1;
		cout << "=======選擇卡牌(" << chooseCard.size() << "/" << Base_Character[character].Hand << ")=======" << endl;
		for (j = 0; j < 6 + Base_Character[character].Deck.size(); j++)
		{
			cout << "                                                                                                                         " << endl;
		}
		for (j = 0; j < Base_Character[character].Deck.size(); j++) 
		{
			setPrintPosition(j * 5, finalCharacter + 12);	cout << "�歈�����";
			setPrintPosition(j * 5, finalCharacter + 13);	cout << "��" << setw(2) << j << "��";
			setPrintPosition(j * 5, finalCharacter + 14);	cout << "�裺�����";
		}
		setPrintPosition(0, 49);
		while (chooseComplete == false)
		{
			setPrintPosition(0, finalCharacter + 10);
			cout << "=======選擇卡牌(" << chooseCard.size() << "/" << Base_Character[character].Hand << ")=======" << endl;
			for (int k = 0; k < chooseCard.size(); k++) 
			{
				SetColor(10);
				setPrintPosition(chooseCard[k] * 5, finalCharacter + 12);	cout << "�歈�����";
				setPrintPosition(chooseCard[k] * 5, finalCharacter + 13);	cout << "��" << setw(2) << chooseCard[k] << "��";
				setPrintPosition(chooseCard[k] * 5, finalCharacter + 14);	cout << "�裺�����";
			}
			SetColor(14);
			setPrintPosition(nowCard * 5, finalCharacter + 12);	cout << "�歈�����";
			setPrintPosition(nowCard * 5, finalCharacter + 13);	cout << "��" << setw(2) << nowCard << "��";
			setPrintPosition(nowCard * 5, finalCharacter + 14);	cout << "�裺�����";
			SetColor(7);
			setPrintPosition(0, 49);
			if (nowCard != card)
			{
				SetColor(7);
				setPrintPosition(card * 5, finalCharacter + 12);	cout << "�歈�����";
				setPrintPosition(card * 5, finalCharacter + 13);	cout << "��" << setw(2) << card << "��";
				setPrintPosition(card * 5, finalCharacter + 14);	cout << "�裺�����";
				SetColor(14);
				setPrintPosition(nowCard * 5, finalCharacter + 12);	cout << "�歈�����";
				setPrintPosition(nowCard * 5, finalCharacter + 13);	cout << "��" << setw(2) << nowCard << "��";
				setPrintPosition(nowCard * 5, finalCharacter + 14);	cout << "�裺�����";
				setPrintPosition(0, finalCharacter + 16);
				SetColor(7); 
				cout << "                                                                                                                         " << endl;
				setPrintPosition(0, finalCharacter + 16);
				cout << "編號：	" << Base_Character[character].Deck[nowCard].ID << "	敏捷值：" << Base_Character[character].Deck[nowCard].Dexterity_Value << "	上：";
				for (int k = 0; k < Base_Character[nowCharacter].Deck[nowCard].MovementUp.size(); k++)
				{
					cout << Base_Character[character].Deck[nowCard].MovementUp[k].Movement << " " << Base_Character[character].Deck[nowCard].MovementUp[k].Movement_Value << " ";
					if (Base_Character[character].Deck[nowCard].MovementUp[k].Movement == "attack")
						cout << "range " << Base_Character[character].Deck[nowCard].MovementUp[k].range << " ";
				}
				cout << " | 下：";
				for (int k = 0; k < Base_Character[character].Deck[nowCard].MovementDown.size(); k++)
				{
					cout << Base_Character[character].Deck[nowCard].MovementDown[k].Movement << " " << Base_Character[character].Deck[nowCard].MovementDown[k].Movement_Value << " ";
					if (Base_Character[character].Deck[nowCard].MovementDown[k].Movement == "attack")
						cout << "range " << Base_Character[character].Deck[nowCard].MovementDown[k].range << " ";
				}
				setPrintPosition(0, 49);
				card = nowCard;
			}
			else
			{
				switch (keyBoard(_getch()))
				{
				case 'a':
					nowCard--;
					if (nowCard < 0)
						nowCard = Base_Character[character].Deck.size() - 1;
					break;
				case 'd':
					nowCard++;
					if (nowCard == Base_Character[character].Deck.size())
						nowCard = 0;
					break;
				case 13:
					//跳出
					vector<int> ::iterator in;
					if ((in = find(chooseCard.begin(), chooseCard.end(), nowCard)) != chooseCard.end())
						chooseCard.erase(in);
					else
						chooseCard.push_back(nowCard);
					if (chooseCard.size() == Base_Character[character].Hand)
					{
						setPrintPosition(0, finalCharacter + 10);
						for (int k = 0; k < 7; k++)
						{
							cout << "                                                                                                                         " << endl;
						}
						setPrintPosition(0, finalCharacter + 10);
						cout << "�歈�������������������������������������������������������������" << endl;
						cout << "�蠸T定選擇這些卡牌來進行遊戲嗎？��" << endl;
						cout << "�裺�������������������������������������������������������������" << endl << endl;
						cout << "確定	取消";
						bool doubleCheck = false, doubleChoose = false, nowDoubleChoose = true;
						while (doubleCheck == false)
						{
							if (nowDoubleChoose != doubleChoose)
							{
								if (nowDoubleChoose == true)
								{
									setPrintPosition(0, finalCharacter + 14);
									SetColor(240);	cout << "確定";
									SetColor(7);	cout << "	取消";
								}
								else
								{
									setPrintPosition(0, finalCharacter + 14);
									SetColor(7);	cout << "確定	";
									SetColor(240);	cout << "取消";
								}
								SetColor(7); setPrintPosition(0, 49);
								doubleChoose = nowDoubleChoose;
							}
							else
							{
								switch (keyBoard(_getch()))
								{
								case'a':
								case'd':
									if (doubleChoose == true)
										nowDoubleChoose = false;
									else
										nowDoubleChoose = true;
									break;
								case 13:
									doubleCheck = true;
									if (nowDoubleChoose == true)
									{
										//選擇完畢
										Character newCharacter = Base_Character[character];
										newCharacter.ID = 'A' + i;
										for (int k = 0; k < chooseCard.size(); k++) 
										{
											newCharacter.Deck[chooseCard[k]].status = 1;
										}
										play_Character.push_back(newCharacter);
										chooseComplete = true;
									}
									else
									{
										chooseCard.erase(in = find(chooseCard.begin(), chooseCard.end(), nowCard));
										setPrintPosition(0, finalCharacter + 10);
										for (j = 0; j < 7 + Base_Character[character].Deck.size(); j++)
										{
											cout << "                                                                                                                         " << endl;
										}
										setPrintPosition(0, finalCharacter + 10);
										cout << "=======選擇卡牌(" << chooseCard.size() << "/" << Base_Character[character].Hand << ")=======" << endl;
										for (j = 0; j < Base_Character[character].Deck.size(); j++)
										{
											setPrintPosition(j * 5, finalCharacter + 12);	cout << "�歈�����";
											setPrintPosition(j * 5, finalCharacter + 13);	cout << "��" << setw(2) << j << "��";
											setPrintPosition(j * 5, finalCharacter + 14);	cout << "�裺�����";
										}
										cout << endl << endl << "編號：	" << Base_Character[character].Deck[nowCard].ID << "	敏捷值：" << Base_Character[character].Deck[nowCard].Dexterity_Value << "	上：";
										for (int k = 0; k < Base_Character[nowCharacter].Deck[nowCard].MovementUp.size(); k++)
										{
											cout << Base_Character[character].Deck[nowCard].MovementUp[k].Movement << " " << Base_Character[character].Deck[nowCard].MovementUp[k].Movement_Value << " ";
											if (Base_Character[character].Deck[nowCard].MovementUp[k].Movement == "attack")
												cout << "range " << Base_Character[character].Deck[nowCard].MovementUp[k].range << " ";
										}
										cout << " | 下：";
										for (int k = 0; k < Base_Character[character].Deck[nowCard].MovementDown.size(); k++)
										{
											cout << Base_Character[character].Deck[nowCard].MovementDown[k].Movement << " " << Base_Character[character].Deck[nowCard].MovementDown[k].Movement_Value << " ";
											if (Base_Character[character].Deck[nowCard].MovementDown[k].Movement == "attack")
												cout << "range " << Base_Character[character].Deck[nowCard].MovementDown[k].range << " ";
										}
										setPrintPosition(0, 49);
									}
									break;
								}
							}
						}
					}
					break;
				}
			}
		}
		system("cls");
	}
	
}
void choose_Start_Position_UI(vector<Character>& play_Character, vector<Ethnicity>& Monster, Map& Game_Map) 
{
	sort(Game_Map.Init_Pos.begin(), Game_Map.Init_Pos.end(), compare);
	for (int i = 0; i < Game_Map.Init_Pos.size(); i++)
	{
		Game_Map.check_road(Game_Map.Init_Pos[i].x, Game_Map.Init_Pos[i].y);
	}
	system("cls");
	Game_Map.print_Map_UI(play_Character, Monster);
	for (int i = 0; i < play_Character.size(); i++) 
	{
		setPrintPosition(0, Game_Map.High + 4);
		cout << "======請選擇角色起始位置======" << endl << endl;
		cout << "※使用鍵盤wasd移動＊在"; SetColor(170); cout << "  "; SetColor(7); cout << "區域選擇起始位置，並按下Enter鍵確認";
		setPrintPosition(0, 49);
		bool chooseComplete = false;
		int x = Game_Map.Init_Pos[0].x, y = Game_Map.Init_Pos[0].y, nowx = Game_Map.Init_Pos[0].x, nowy = Game_Map.Init_Pos[0].y;
		setPrintPosition((nowx + 1) * 2, nowy);
		SetColor(160);	cout << "＊";	SetColor(7);
		setPrintPosition(0, 49);
		while (chooseComplete == false) 
		{
			if (x != nowx || y != nowy) 
			{
				setPrintPosition((x + 1) * 2, y);
				SetColor(170);	cout << "  "; 
				setPrintPosition((nowx + 1) * 2, nowy);
				SetColor(160);	cout << "＊";	SetColor(7);
				setPrintPosition(0, 49);
				x = nowx; y = nowy;
			}
			else 
			{
				switch (keyBoard(_getch())) 
				{
				case 'w':
					if (Game_Map.Game_Map[nowy - 1][nowx] == 5)
						nowy--;
					break;
				case 's':
					if (Game_Map.Game_Map[nowy + 1][nowx] == 5)
						nowy++;
					break;
				case 'a':
					if (Game_Map.Game_Map[nowy][nowx - 1] == 5)
						nowx--;
					break;
				case 'd':
					if (Game_Map.Game_Map[nowy][nowx + 1] == 5)
						nowx++;
					break;
				case 13:
					//完成選擇
					play_Character[i].position.y = nowy;
					play_Character[i].position.x = nowx;
					Game_Map.Game_Map[nowy][nowx] = 1;
					for (int j = 0; j < Game_Map.Init_Pos.size(); j++)
					{
						if (Game_Map.Init_Pos[j].x == nowx && Game_Map.Init_Pos[j].y == nowy)
						{
							Game_Map.Init_Pos.erase(Game_Map.Init_Pos.begin() + j);
						}
					}
					chooseComplete = true;
					break;
				}
			}
		}
		setPrintPosition((nowx + 1) * 2, nowy);
		SetColor(10);	cout << "p" << play_Character[i].ID;	SetColor(7);
		setPrintPosition(0, Game_Map.High + 4);
		for (int k = 0; k < 3; k++)
			cout << "                                                                                                                         " << endl;
		setPrintPosition(0, 49);
	}
	for (int i = 0; i < Game_Map.High; i++)
	{
		for (int j = 0; j < Game_Map.Width; j++)
		{
			if (Game_Map.Game_Map[i][j] == 5)
			{
				Game_Map.Game_Map[i][j] = 1;
				setPrintPosition((j + 1) * 2, i);
				cout << "□";
				setPrintPosition(0, 49);
			}

		}
	}
}
void main_Battle_UI(vector<Character>& play_Character, vector<Ethnicity>& Monster, Map& Game_Map) 
{
	bool* already_played = nullptr;
	srand(time(NULL));
	int round = 1;
	vector<string> game_Massage_string;
	while (end_Game_UI(play_Character, Monster, Game_Map, true))
	{
		setPrintPosition(0, Game_Map.High + 4);
		for (int i = 0; i < 30; i++) 
		{
			cout << "                                                                                                                         " << endl;
		}
		setPrintPosition(0, 0);
		setPrintPosition(0, Game_Map.High + 4);
		cout << "========第" << round << "回合========" << endl << endl;
		cout << "行動順序：" << endl << endl;
		cout << "======玩家準備階段======" << endl << endl; 
		{
			already_played = new bool[play_Character.size()];
			for (int i = 0; i < play_Character.size(); i++)
			{
				already_played[i] = false;
			}
			cout << "角色：";
			for (int i = 0; i < play_Character.size(); i++)
				cout << "  " << play_Character[i].ID;
			bool chooseComplete = false;
			int Character = 0, nowCharacter = 0;
			setPrintPosition(8 + 3 * Character, Game_Map.High + 10); SetColor(7);
			cout << play_Character[Character].ID;
			setPrintPosition(8 + 3 * nowCharacter, Game_Map.High + 10); SetColor(240);
			cout << play_Character[nowCharacter].ID;
			setPrintPosition(0, 49); SetColor(7);
			Character = nowCharacter;
			while (chooseComplete == false)
			{
				if (Character != nowCharacter)
				{
					setPrintPosition(8 + 3 * Character, Game_Map.High + 10); SetColor(7);
					cout << play_Character[Character].ID;
					setPrintPosition(8 + 3 * nowCharacter, Game_Map.High + 10); SetColor(240);
					cout << play_Character[nowCharacter].ID;
					setPrintPosition(0, 49); SetColor(7);
					Character = nowCharacter;
				}
				else
				{
					switch (keyBoard(_getch()))
					{
					case 'a':
						nowCharacter--;
						if (nowCharacter < 0)
							nowCharacter = play_Character.size() - 1;
						break;
					case 'd':
						nowCharacter++;
						if (nowCharacter > play_Character.size() - 1)
							nowCharacter = 0;
						break;
					case 13:
						int hand = 0, dedeck = 0;
						for (int i = 0; i < play_Character[nowCharacter].Deck.size(); i++)
						{
							if (play_Character[nowCharacter].Deck[i].status == 1)
								hand++;
							else if (play_Character[nowCharacter].Deck[i].status == 2)
								dedeck++;
						}
						setPrintPosition(0, Game_Map.High + 12);
						cout << "�歈�����������������������������" << endl;
						cout << "��  →"; if (hand < 2 || already_played[nowCharacter])SetColor(8); cout << "出牌"; SetColor(7); cout << "      ��" << endl;
						cout << "��    "; if (dedeck < 2 || already_played[nowCharacter])SetColor(8); cout << "長休"; SetColor(7); cout << "      ��" << endl;
						cout << "��    檢查手牌  ��" << endl;
						cout << "��    場面狀態  ��" << endl;
						cout << "�裺�����������������������������" << endl << endl;
						bool chooseComplete2 = false; int nowChoose = 0, Choose = 0;
						while (chooseComplete2 == false)
						{
							if (nowChoose != Choose)
							{
								setPrintPosition(3, Game_Map.High + 13 + Choose);		cout << "  ";
								setPrintPosition(3, Game_Map.High + 13 + nowChoose);	cout << "→";
								setPrintPosition(0, 49);
								Choose = nowChoose;
							}
							else
							{
								switch (keyBoard(_getch()))
								{
								case 'w':
									nowChoose--;
									if (nowChoose < 0)
										nowChoose = 3;
									break;
								case 's':
									nowChoose++;
									if (nowChoose > 3)
										nowChoose = 0;
									break;
								case 13:
									switch (nowChoose)
									{
									case 0:
										if (hand > 1 && !already_played[nowCharacter])
										{
											setPrintPosition(0, Game_Map.High + 12);
											for (int i = 0; i < 6; i++)
											{
												cout << "                                                                                                                         " << endl;
											}
											setPrintPosition(0, Game_Map.High + 12);
											player_Use_Card_UI(play_Character, nowCharacter, Game_Map.High + 12);
											already_played[nowCharacter] = true;
											chooseComplete2 = true;
											setPrintPosition(0, Game_Map.High + 12);
											for (int i = 0; i < 10; i++)
											{
												cout << "                                                                                                                         " << endl;
											}
											setPrintPosition(0, 49);
										}
										break;
									case 1:
										if (dedeck > 1 && !already_played[nowCharacter])
										{
											play_Character[nowCharacter].Command[0] = -1;
											play_Character[nowCharacter].Rest = true;
											already_played[nowCharacter] = true;
											chooseComplete2 = true;
											setPrintPosition(0, Game_Map.High + 12);
											for (int i = 0; i < 6; i++)
											{
												cout << "                                                                                                                         " << endl;
											}
											setPrintPosition(0, 49);
										}
										break;
									case 2:
										setPrintPosition(0, Game_Map.High + 12);
										for (int i = 0; i < 6; i++)
										{
											cout << "                                                                                                                         " << endl;
										}
										setPrintPosition(0, Game_Map.High + 12);
										check_hand_UI(play_Character, nowCharacter, Game_Map.High + 12);
										setPrintPosition(0, Game_Map.High + 12);
										for (int i = 0; i < 10; i++)
										{
											cout << "                                                                                                                         " << endl;
										}
										setPrintPosition(0, Game_Map.High + 12);
										cout << "�歈�����������������������������" << endl;
										cout << "��    "; if (hand < 2 || already_played[nowCharacter])SetColor(8); cout << "出牌"; SetColor(7); cout << "      ��" << endl;
										cout << "��    "; if (dedeck < 2 || already_played[nowCharacter])SetColor(8); cout << "長休"; SetColor(7); cout << "      ��" << endl;
										cout << "��    檢查手牌  ��" << endl;
										cout << "��    場面狀態  ��" << endl;
										cout << "�裺�����������������������������" << endl << endl;
										setPrintPosition(3, Game_Map.High + 13 + nowChoose); cout << "→";
										setPrintPosition(0, 49);
										break;
									case 3:
										setPrintPosition(0, Game_Map.High + 12);
										for (int i = 0; i < 6; i++)
										{
											cout << "                                                                                                                         " << endl;
										}
										int clean = game_Massage(play_Character, Monster, Game_Map, game_Massage_string);
										cout << endl;
										setPrintPosition(0, Game_Map.High + 12 + clean + 2);
										cout << "※按下Enter鍵返回";
										while (1) 
										{
											if (keyBoard(_getch()) == 13)
												break;
										}
										setPrintPosition(0, Game_Map.High + 12);
										for (int i = 0; i < clean + 4; i++)
										{
											cout << "                                                                                                                         " << endl;
										}
										setPrintPosition(0, Game_Map.High + 12);
										cout << "�歈�����������������������������" << endl;
										cout << "��    "; if (hand < 2 || already_played[nowCharacter])SetColor(8); cout << "出牌"; SetColor(7); cout << "      ��" << endl;
										cout << "��    "; if (dedeck < 2 || already_played[nowCharacter])SetColor(8); cout << "長休"; SetColor(7); cout << "      ��" << endl;
										cout << "��    檢查手牌  ��" << endl;
										cout << "��    場面狀態  ��" << endl;
										cout << "�裺�����������������������������" << endl << endl;
										setPrintPosition(3, Game_Map.High + 13 + nowChoose); cout << "→";
										setPrintPosition(0, 0);
										setPrintPosition(0, 49);
										break;
									}
									break;
								case 27:
									chooseComplete2 = true;
									setPrintPosition(0, Game_Map.High + 12);
									for (int i = 0; i < 6; i++)
									{
										cout << "                                                                                                                         " << endl;
									}
									setPrintPosition(0, 49);
									break;
								}
							}
						}
						break;
					}
				}
				bool all_Complete = true;
				for (int i = 0; i < play_Character.size(); i++)
				{
					if (already_played[i] == false)
						all_Complete = false;
				}
				if (all_Complete == true)
				{
					chooseComplete = true;
					delete[] already_played;
				}
			}
			setPrintPosition(0, Game_Map.High + 8);
			for (int i = 0; i < 9; i++)
			{
				cout << "                                                                                                                         " << endl;
			}
		}
		setPrintPosition(0, Game_Map.High + 8);
		cout << "======敵人準備階段======" << endl << endl; 
		{
			for (int i = 0; i < Monster.size(); i++)	//第i個種族
			{
				bool all_deactivate = true;
				for (int j = 0; j < Monster[i].Creature_List.size(); j++)
				{
					if (Monster[i].Creature_List[j].active == true)
						all_deactivate = false;
				}
				if (all_deactivate == true)
				{
					Monster[i].Command = -1;
					continue;
				}
				setPrintPosition(0, Game_Map.High + 10);
				cout << "====" << Monster[i].Ethnicity_Name << "準備====" << endl;
				Sleep(1000);
				bool finished = false;
				do
				{
					int choose = rand() % (Monster[i].Deck.size());
					if (Monster[i].Deck[choose].status == 0)	//確認是否在牌組中
					{
						Monster[i].Deck[choose].status = 2;	//改成出牌中
						Monster[i].Command = choose;
						if (Monster[i].Deck[choose].Shuffle_Mark) {
							Monster[i].Shuffle_Mark = true;
						}
						finished = true;		//結束選擇
					}
				} while (finished == false);		//結束選擇即跳出此迴圈
			}
			setPrintPosition(0, Game_Map.High + 8);
			for (int i = 0; i < 9; i++)
			{
				cout << "                                                                                                                         " << endl;
			}
		}
		setPrintPosition(0, Game_Map.High + 8);
		cout << "======行動排序階段======" << endl << endl;
		vector<char> attack_Sort;	//以敏捷值排序的攻擊順序
		{
			//先排角色
			attack_Sort.push_back(play_Character[0].ID);	//將角色A加入排序
			for (int i = 1; i < play_Character.size(); i++)
			{
				for (int j = 0; j < attack_Sort.size(); j++)
				{
					int fir_Dex, sec_Dex;
					if (play_Character[i].Command[0] == -1)
					{
						fir_Dex = 99; sec_Dex = 99;
					}
					else
					{
						fir_Dex = play_Character[i].Deck[play_Character[i].Command[0]].Dexterity_Value;
						sec_Dex = play_Character[i].Deck[play_Character[i].Command[1]].Dexterity_Value;
					}
					if (fir_Dex < get_Character_Dex(play_Character, attack_Sort[j], 0))
					{
						attack_Sort.insert(attack_Sort.begin() + j, play_Character[i].ID);
						break;
					}
					else if (fir_Dex == get_Character_Dex(play_Character, attack_Sort[j], 0))
					{
						if (sec_Dex < get_Character_Dex(play_Character, attack_Sort[j], 1))
						{
							attack_Sort.insert(attack_Sort.begin() + j, play_Character[i].ID);
							break;
						}
						else if (j + 1 == attack_Sort.size()) //第二個敏捷值仍然相同或較大，由於是照角色字母順序加進排序，所以直接跳到下一位判斷，如果此位是最後一位，則直接將角色加到排序最後一位
						{
							attack_Sort.push_back(play_Character[i].ID);
							break;
						}
						else {
							continue;
						}
					}
					else if (j + 1 == attack_Sort.size()) //排序中的最後一位
					{
						attack_Sort.push_back(play_Character[i].ID);
						break;
					}
					else
					{

					}
				}
			}
			//再排怪物		由於怪物是以種族來排，所以序列裡我將怪物的代號以('a'+i代替)，到時候寫攻擊時要稍微注意
			for (int i = 0; i < Monster.size(); i++)
			{
				if (Monster[i].Command == -1)
					continue;
				/*bool all_active = true;
				for (int k = 0; k < Monster[i].Creature_List.size(); k++)
				{
					if (Monster[i].Creature_List[k].active == false)
						all_active = false;
				}*/
				for (int j = 0; j < attack_Sort.size(); j++)
				{
					if (attack_Sort[j] >= 'A' && attack_Sort[j] <= 'Z') //判斷是否是玩家角色
					{
						if (Monster[i].Deck[Monster[i].Command].Dexterity_Value < get_Character_Dex(play_Character, attack_Sort[j], 0))
						{
							attack_Sort.insert(attack_Sort.begin() + j, 'a' + i);
							break;
						}
					}
					else
					{
						if (Monster[i].Deck[Monster[i].Command].Dexterity_Value < get_Monster_Dex(Monster, attack_Sort[j]))
						{
							attack_Sort.insert(attack_Sort.begin() + j, 'a' + i);
							break;
						}
						else if (Monster[i].Deck[Monster[i].Command].Dexterity_Value == get_Monster_Dex(Monster, attack_Sort[j]) && 'a' + i < attack_Sort[j])
						{
							attack_Sort.insert(attack_Sort.begin() + j, 'a' + i);
							break;
						}
					}
					if (j + 1 == attack_Sort.size())
					{
						attack_Sort.push_back('a' + i);
						break;
					}
				}
				//}
			}
			//檢查排序
			setPrintPosition(0, Game_Map.High + 6);
			cout << "                                                                                                                         " << endl;
			setPrintPosition(0, Game_Map.High + 6);
			cout << "行動順序：";
			for (int i = 0; i < attack_Sort.size(); i++)
			{
				cout << "<<";
				if (attack_Sort[i] >= 'A' && attack_Sort[i] <= 'Z')
				{
					for (int j = 0; j < play_Character.size(); j++) 
					{
						if (play_Character[j].ID == attack_Sort[i]) 
						{
							cout << play_Character[j].ID << "(" << play_Character[j].Command[0] << "/" << play_Character[j].Command[1] << ")";
						}
					}
				}
				else
				{
					cout << Monster[attack_Sort[i] - 'a'].Ethnicity_Name << "(" << Monster[attack_Sort[i] - 'a'].Command << ")";
				}
			}
		}
		//戰鬥部分
		{
			for (int i = 0; i < attack_Sort.size(); i++)
			{
				if (attack_Sort[i] <= 'Z' && attack_Sort[i] >= 'A')		//角色行動
				{
					for (int j = 0; j < play_Character.size(); j++)
					{
						if (play_Character[j].ID == attack_Sort[i])
						{
							setPrintPosition(0, Game_Map.High + 8);
							cout << "======玩家行動階段======" << endl << endl;
							players_round_UI(play_Character, play_Character[j], Monster, Game_Map, Game_Map.High + 10,game_Massage_string);
						}
					}
				}
				else //怪物行動
				{
					for (int k = 0; k < Monster[attack_Sort[i] - 'a'].Creature_List.size(); k++)
					{
						setPrintPosition(0, Game_Map.High + 8);
						cout << "======敵人行動階段======" << endl << endl;
						monsters_round_UI(play_Character, Monster[attack_Sort[i] - 'a'], Monster[attack_Sort[i] - 'a'].Creature_List[k], Game_Map, Monster, attack_Sort, Game_Map.High + 10,game_Massage_string);
					}
				}
			}
		}
		attack_Sort.clear();
		end_round_UI(play_Character, Monster, Game_Map);
		round++;
	}

}
int game_Massage(vector<Character>& play_Character, vector<Ethnicity>& Monster, Map& Game_Map, vector<string>& game_Massage_string)
{
	setPrintPosition(0, Game_Map.High + 12);
	cout << "======遊戲資訊======" << endl << endl;
	cout << "======玩家資訊======            ======敵方資訊======            ======行動歷史======";
	for (int i = 0; i < play_Character.size(); i++) 
	{
		cout << endl << endl << "角色" << play_Character[i].ID << "：" << endl;
		cout << setw(10) << "名稱：" << play_Character[i].Character_name << endl;
		cout << setw(10) << "血量：(" << play_Character[i].Hp << "/" << play_Character[i].Max_HP << ")" << endl;
		cout << setw(10) << "護盾：" << play_Character[i].Shield << endl;
		int onHand = 0, used = 0;
		for (int j = 0; j < play_Character[i].Deck.size(); j++) 
		{
			if (play_Character[i].Deck[j].status == 1)
				onHand++;
			else if (play_Character[i].Deck[j].status == 2)
				used++;
		}
		cout << setw(10) << "手牌數：" << onHand << endl;
		cout << setw(10) << "棄牌數：" << used;
	}
	int printPoint = Game_Map.High + 16, allPoint = 0;
	for (int i = 0; i < Monster.size(); i++)
	{
		bool all_unactive = true;
		for (int j = 0; j < Monster[i].Creature_List.size(); j++) 
		{
			if (Monster[i].Creature_List[j].active == true)
				all_unactive = false;
		}
		if (all_unactive == true)
			continue;
		setPrintPosition(32, printPoint); cout << "種族名：" << Monster[i].Ethnicity_Name;
		for (int j = 0; j < Monster[i].Creature_List.size(); j++) 
		{
			if (Monster[i].Creature_List[j].active == false)
				continue;
			setPrintPosition(32, printPoint + 1); cout << setw(10) << "名稱：" << Monster[i].Creature_List[j].icon;
			setPrintPosition(32, printPoint + 2); cout << setw(14) << "血量：(" << Monster[i].Creature_List[j].Hp << "/" << Monster[i].Creature_List[j].Max_HP << ")";
			setPrintPosition(32, printPoint + 3); cout << setw(14) << "護盾：" << Monster[i].Creature_List[j].Shield;
			setPrintPosition(32, printPoint + 4); cout << setw(14) << "攻擊力：" << Monster[i].Creature_List[j].Damage;
			setPrintPosition(32, printPoint + 5); cout << setw(14) << "攻擊範圍：" << Monster[i].Creature_List[j].Range;
			printPoint += 6;
		}
		int inDeck = 0, used = 0;
		for (int j = 0; j < Monster[i].Deck.size(); j++)
		{
			if (Monster[i].Deck[j].status == 0)
				inDeck++;
			else if (Monster[i].Deck[j].status == 1)
				used++;
		}
		setPrintPosition(32, printPoint); cout << setw(10) << "牌堆：" << inDeck;
		setPrintPosition(32, printPoint + 1); cout << setw(10) << "棄牌數：" << used;
		printPoint += 2;
	}
	allPoint = printPoint - (Game_Map.High + 12);
	printPoint = Game_Map.High + 16;
	if (game_Massage_string.size() > 17) 
	{
		int eraser = game_Massage_string.size() - 17;
		game_Massage_string.erase(game_Massage_string.begin(), game_Massage_string.begin() + eraser);
	}
	for (int i = 0; i < game_Massage_string.size(); i++) 
	{
		setPrintPosition(64, printPoint + i);	cout << game_Massage_string[i];
	}
	if (allPoint > 30)
		return allPoint;
	else 
		return 30;
}
void check_hand_UI(vector<Character>& play_Character,int character,int printPoint) 
{
	for (int i = 0; i < play_Character[character].Deck.size(); i++)
	{
		if (play_Character[character].Deck[i].status == 1)
			SetColor(7);
		else if (play_Character[character].Deck[i].status == 2)
			SetColor(12);
		else
			SetColor(8);
		setPrintPosition(i * 5, printPoint + 1);	cout << "�歈�����";
		setPrintPosition(i * 5, printPoint + 2);	cout << "��" << setw(2) << play_Character[character].Deck[i].ID << "��";
		setPrintPosition(i * 5, printPoint + 3);	cout << "�裺�����";
	}
	SetColor(7); setPrintPosition(0, printPoint + 7);	cout << "※卡片狀態：	白色->手上	紅色->棄牌區	灰色->場外" << endl;
	cout << endl << "※按下Enter鍵返回";
	bool checkComplete = false;
	int nowCard = 0, card = 1;
	setPrintPosition(0, 49);
	while (checkComplete == false)
	{
		if (nowCard != card)
		{
			if (play_Character[character].Deck[card].status == 1)
				SetColor(7);
			else if (play_Character[character].Deck[card].status == 2)
				SetColor(12);
			else
				SetColor(8);
			setPrintPosition(card * 5, printPoint + 1);	cout << "�歈�����";
			setPrintPosition(card * 5, printPoint + 2);	cout << "��" << setw(2) << card << "��";
			setPrintPosition(card * 5, printPoint + 3);	cout << "�裺�����";
			SetColor(14);
			setPrintPosition(nowCard * 5, printPoint + 1);	cout << "�歈�����";
			setPrintPosition(nowCard * 5, printPoint + 2);	cout << "��" << setw(2) << nowCard << "��";
			setPrintPosition(nowCard * 5, printPoint + 3);	cout << "�裺�����";
			setPrintPosition(0, printPoint + 5);
			SetColor(7);
			cout << "                                                                                                                         " << endl;
			setPrintPosition(0, printPoint + 5);
			cout << "編號：	" << play_Character[character].Deck[nowCard].ID << "	敏捷值：" << play_Character[character].Deck[nowCard].Dexterity_Value << "	上：";
			for (int k = 0; k < play_Character[character].Deck[nowCard].MovementUp.size(); k++)
			{
				cout << play_Character[character].Deck[nowCard].MovementUp[k].Movement << " " << play_Character[character].Deck[nowCard].MovementUp[k].Movement_Value << " ";
				if (play_Character[character].Deck[nowCard].MovementUp[k].Movement == "attack")
					cout << "range " << play_Character[character].Deck[nowCard].MovementUp[k].range << " ";
			}
			cout << " | 下：";
			for (int k = 0; k < play_Character[character].Deck[nowCard].MovementDown.size(); k++)
			{
				cout << play_Character[character].Deck[nowCard].MovementDown[k].Movement << " " << play_Character[character].Deck[nowCard].MovementDown[k].Movement_Value << " ";
				if (play_Character[character].Deck[nowCard].MovementDown[k].Movement == "attack")
					cout << "range " << play_Character[character].Deck[nowCard].MovementDown[k].range << " ";
			}
			setPrintPosition(0, 49);
			card = nowCard;
		}
		else
		{
			switch (keyBoard(_getch()))
			{
			case 'a':
				nowCard--;
				if (nowCard < 0)
					nowCard = play_Character[character].Deck.size() - 1;
				break;
			case 'd':
				nowCard++;
				if (nowCard == play_Character[character].Deck.size())
					nowCard = 0;
				break;
			case 13:
				return;
				break;
			}
		}
	}
}
void player_Use_Card_UI(vector<Character>& play_Character, int character, int printPoint)
{
	cout << "====請選擇要出的牌====";
	vector<int> can_Use_Card;
	for (int i = 0; i < play_Character[character].Deck.size(); i++)
	{
		if (play_Character[character].Deck[i].status == 1)
			can_Use_Card.push_back(i);
	}
	for (int i = 0; i < can_Use_Card.size(); i++)
	{
		SetColor(7);
		setPrintPosition(i * 5, printPoint + 2);	cout << "�歈�����";
		setPrintPosition(i * 5, printPoint + 3);	cout << "��" << setw(2) << play_Character[character].Deck[can_Use_Card[i]].ID << "��";
		setPrintPosition(i * 5, printPoint + 4);	cout << "�裺�����";
	}
	SetColor(7); setPrintPosition(0, printPoint + 8);
	cout << "※w：選擇	s：取消	Enter：確認";
	bool checkComplete = false;
	int nowCard = 0, card = 0;
	vector<int> use_card;
	vector<int>::iterator in;
	SetColor(7);
	setPrintPosition(card * 5, printPoint + 2);	cout << "�歈�����";
	setPrintPosition(card * 5, printPoint + 3);	cout << "��" << setw(2) << can_Use_Card[card] << "��";
	setPrintPosition(card * 5, printPoint + 4);	cout << "�裺�����";
	SetColor(14);
	setPrintPosition(nowCard * 5, printPoint + 2);	cout << "�歈�����";
	setPrintPosition(nowCard * 5, printPoint + 3);	cout << "��" << setw(2) << can_Use_Card[nowCard] << "��";
	setPrintPosition(nowCard * 5, printPoint + 4);	cout << "�裺�����";
	setPrintPosition(0, printPoint + 6);
	SetColor(7);
	cout << "                                                                                                                         " << endl;
	setPrintPosition(0, printPoint + 6);
	cout << "編號：	" << play_Character[character].Deck[can_Use_Card[nowCard]].ID << "	敏捷值：" << play_Character[character].Deck[can_Use_Card[nowCard]].Dexterity_Value << "	上：";
	for (int k = 0; k < play_Character[character].Deck[can_Use_Card[nowCard]].MovementUp.size(); k++)
	{
		cout << play_Character[character].Deck[can_Use_Card[nowCard]].MovementUp[k].Movement << " " << play_Character[character].Deck[can_Use_Card[nowCard]].MovementUp[k].Movement_Value << " ";
		if (play_Character[character].Deck[can_Use_Card[nowCard]].MovementUp[k].Movement == "attack")
			cout << "range " << play_Character[character].Deck[can_Use_Card[nowCard]].MovementUp[k].range << " ";
	}
	cout << " | 下：";
	for (int k = 0; k < play_Character[character].Deck[can_Use_Card[nowCard]].MovementDown.size(); k++)
	{
		cout << play_Character[character].Deck[can_Use_Card[nowCard]].MovementDown[k].Movement << " " << play_Character[character].Deck[can_Use_Card[nowCard]].MovementDown[k].Movement_Value << " ";
		if (play_Character[character].Deck[can_Use_Card[nowCard]].MovementDown[k].Movement == "attack")
			cout << "range " << play_Character[character].Deck[can_Use_Card[nowCard]].MovementDown[k].range << " ";
	}
	setPrintPosition(0, 49);
	card = nowCard;
	setPrintPosition(0, 49);
	while (checkComplete == false)
	{
		for (int i = 0; i < use_card.size(); i++) 
		{
			setPrintPosition(use_card[i] * 5, printPoint + 1);
			cout << " /" << i + 1 << "\\";
			setPrintPosition(0, 49);
		}
		if (nowCard != card)
		{
			SetColor(7);
			setPrintPosition(card * 5, printPoint + 2);	cout << "�歈�����";
			setPrintPosition(card * 5, printPoint + 3);	cout << "��" << setw(2) << can_Use_Card[card] << "��";
			setPrintPosition(card * 5, printPoint + 4);	cout << "�裺�����";
			SetColor(14);
			setPrintPosition(nowCard * 5, printPoint + 2);	cout << "�歈�����";
			setPrintPosition(nowCard * 5, printPoint + 3);	cout << "��" << setw(2) << can_Use_Card[nowCard] << "��";
			setPrintPosition(nowCard * 5, printPoint + 4);	cout << "�裺�����";
			setPrintPosition(0, printPoint + 6);
			SetColor(7);
			cout << "                                                                                                                         " << endl;
			setPrintPosition(0, printPoint + 6);
			cout << "編號：	" << play_Character[character].Deck[can_Use_Card[nowCard]].ID << "	敏捷值：" << play_Character[character].Deck[can_Use_Card[nowCard]].Dexterity_Value << "	上：";
			for (int k = 0; k < play_Character[character].Deck[can_Use_Card[nowCard]].MovementUp.size(); k++)
			{
				cout << play_Character[character].Deck[can_Use_Card[nowCard]].MovementUp[k].Movement << " " << play_Character[character].Deck[can_Use_Card[nowCard]].MovementUp[k].Movement_Value << " ";
				if (play_Character[character].Deck[can_Use_Card[nowCard]].MovementUp[k].Movement == "attack")
					cout << "range " << play_Character[character].Deck[can_Use_Card[nowCard]].MovementUp[k].range << " ";
			}
			cout << " | 下：";
			for (int k = 0; k < play_Character[character].Deck[can_Use_Card[nowCard]].MovementDown.size(); k++)
			{
				cout << play_Character[character].Deck[can_Use_Card[nowCard]].MovementDown[k].Movement << " " << play_Character[character].Deck[can_Use_Card[nowCard]].MovementDown[k].Movement_Value << " ";
				if (play_Character[character].Deck[can_Use_Card[nowCard]].MovementDown[k].Movement == "attack")
					cout << "range " << play_Character[character].Deck[can_Use_Card[nowCard]].MovementDown[k].range << " ";
			}
			setPrintPosition(0, 49);
			card = nowCard;
		}
		else
		{
			switch (keyBoard(_getch()))
			{
			case 'a':
				nowCard--;
				if (nowCard < 0)
					nowCard = can_Use_Card.size() - 1;
				break;
			case 'd':
				nowCard++;
				if (nowCard == can_Use_Card.size())
					nowCard = 0;
				break;
			case 'w':
				if (use_card.size() ==0) 
				{
					use_card.push_back(nowCard);
				}
				else if (use_card.size() == 1 && use_card[0] != nowCard) 
				{
					use_card.push_back(nowCard);
				}
				break;
			case 's':
				if (find(use_card.begin(),use_card.end(),nowCard)!=use_card.end())
				{
					setPrintPosition(nowCard * 5, printPoint + 1);
					cout << "     ";
					setPrintPosition(0, 49);
					use_card.erase(in = find(use_card.begin(), use_card.end(), nowCard));
				}
				break;
			case 13:
				//完成選擇
				if (use_card.size() == 2) 
				{
					play_Character[character].Command[0] = can_Use_Card[use_card[0]];
					play_Character[character].Command[1] = can_Use_Card[use_card[1]];
					play_Character[character].Deck[can_Use_Card[use_card[0]]].status = 4;
					play_Character[character].Deck[can_Use_Card[use_card[1]]].status = 4;
					return;
				}
				break;
			}
		}
	}
}
void players_round_UI(vector<Character>& play_Character, Character& Character, vector<Ethnicity>& Monster, Map& Game_Map, int printPoint, vector<string>& game_Massage_string) //角色回合 - 已防呆
{
	SetColor(7); setPrintPosition(0, printPoint);
	if (Character.Rest == false)
	{


		cout << "====請選擇" << Character.ID <<"的卡牌指令====" << endl;
		cout << "角色：" << Character.Character_name << "	[hp：(" << Character.Hp << "/" << Character.Max_HP << ")][shield：" << Character.Shield << "]" << endl;
		bool chooseComplete = false;
		int card = 0, movement = 0, now_card = 0, now_movement = 0;
		for (int i = 0; i < 2; i++)
		{
			SetColor(7);
			setPrintPosition(i * 6, printPoint + 2);	cout << "�歈�������";
			setPrintPosition(i * 6, printPoint + 3);	cout << "�� " << setw(2) << Character.Deck[Character.Command[i]].ID << "��";
			setPrintPosition(i * 6, printPoint + 4);	cout << "�� 上��";
			setPrintPosition(i * 6, printPoint + 6);	cout << "�� " << setw(2) << Character.Deck[Character.Command[i]].ID << "��";
			setPrintPosition(i * 6, printPoint + 7);	cout << "�� 下��";
			setPrintPosition(i * 6, printPoint + 8);	cout << "�裺�������";
		}
		setPrintPosition(2 * 6, printPoint + 4);	cout << "�歈�������";
		setPrintPosition(2 * 6, printPoint + 5);	cout << "�� Ｃ��";
		setPrintPosition(2 * 6, printPoint + 6);	cout << "�裺�������";
		SetColor(14);
		setPrintPosition(now_card * 6, printPoint + 2);	cout << "�歈�������";
		setPrintPosition(now_card * 6, printPoint + 3);	cout << "�� " << setw(2) << Character.Deck[Character.Command[now_card]].ID << "��";
		setPrintPosition(now_card * 6, printPoint + 4);	cout << "�� 上��";
		SetColor(7);
		setPrintPosition(0, 49);
		bool check_now = true;
		while (chooseComplete == false) 
		{
			if (now_card == 2 && check_now == true)
			{
				for (int i = 0; i < 2; i++)
				{
					SetColor(7);
					setPrintPosition(i * 6, printPoint + 2);	cout << "�歈�������";
					setPrintPosition(i * 6, printPoint + 3);	cout << "�� " << setw(2) << Character.Deck[Character.Command[i]].ID << "��";
					setPrintPosition(i * 6, printPoint + 4);	cout << "�� 上��";
					setPrintPosition(i * 6, printPoint + 6);	cout << "�� " << setw(2) << Character.Deck[Character.Command[i]].ID << "��";
					setPrintPosition(i * 6, printPoint + 7);	cout << "�� 下��";
					setPrintPosition(i * 6, printPoint + 8);	cout << "�裺�������";
				}
				SetColor(14);
				setPrintPosition(2 * 6, printPoint + 4);	cout << "�歈�������";
				setPrintPosition(2 * 6, printPoint + 5);	cout << "�� Ｃ��";
				setPrintPosition(2 * 6, printPoint + 6);	cout << "�裺�������";
				setPrintPosition(0, printPoint + 10);
				SetColor(7);
				cout << "                                                                                                                         " << endl;
				setPrintPosition(0, 49);
				card = now_card;
				check_now = false;
			}
			else if (now_card != 2 && (now_card != card || now_movement != movement))
			{
				if (card == 2 && now_card == 0)
					card = 1;
				else if (card == 2 && now_card == 1)
					card = 0;
				SetColor(7);
				setPrintPosition(2 * 6, printPoint + 4);	cout << "�歈�������";
				setPrintPosition(2 * 6, printPoint + 5);	cout << "�� Ｃ��";
				setPrintPosition(2 * 6, printPoint + 6);	cout << "�裺�������";
				if (movement == 0) 
				{
					setPrintPosition(card * 6, printPoint + 2);	cout << "�歈�������";
					setPrintPosition(card * 6, printPoint + 3);	cout << "�� " << setw(2) << Character.Deck[Character.Command[card]].ID << "��";
					setPrintPosition(card * 6, printPoint + 4);	cout << "�� 上��";
				}
				else 
				{
					setPrintPosition(card * 6, printPoint + 6);	cout << "�� " << setw(2) << Character.Deck[Character.Command[card]].ID << "��";
					setPrintPosition(card * 6, printPoint + 7);	cout << "�� 下��";
					setPrintPosition(card * 6, printPoint + 8);	cout << "�裺�������";
				}
				SetColor(14);
				if (now_movement == 0)
				{
					setPrintPosition(now_card * 6, printPoint + 2);	cout << "�歈�������";
					setPrintPosition(now_card * 6, printPoint + 3);	cout << "�� " << setw(2) << Character.Deck[Character.Command[now_card]].ID << "��";
					setPrintPosition(now_card * 6, printPoint + 4);	cout << "�� 上��";
				}
				else
				{
					setPrintPosition(now_card * 6, printPoint + 6);	cout << "�� " << setw(2) << Character.Deck[Character.Command[now_card]].ID << "��";
					setPrintPosition(now_card * 6, printPoint + 7);	cout << "�� 下��";
					setPrintPosition(now_card * 6, printPoint + 8);	cout << "�裺�������";
				}
				setPrintPosition(0, printPoint + 10);
				SetColor(7);
				cout << "                                                                                                                         " << endl;
				setPrintPosition(0, printPoint + 10);
				cout << "攻擊指令 -> ";
				if (now_movement == 0) 
				{
					for (int k = 0; k < Character.Deck[Character.Command[now_card]].MovementUp.size(); k++)
					{
						cout << Character.Deck[Character.Command[now_card]].MovementUp[k].Movement << " " << Character.Deck[Character.Command[now_card]].MovementUp[k].Movement_Value << " ";
						if (Character.Deck[Character.Command[now_card]].MovementUp[k].Movement == "attack")
							cout << "range " << Character.Deck[Character.Command[now_card]].MovementUp[k].range << " ";
					}
					if (now_card == 0)
						card = 1;
					else
						card = 0;
					cout << "	+	";
					for (int k = 0; k < Character.Deck[Character.Command[card]].MovementDown.size(); k++)
					{
						cout << Character.Deck[Character.Command[card]].MovementDown[k].Movement << " " << Character.Deck[Character.Command[card]].MovementDown[k].Movement_Value << " ";
						if (Character.Deck[Character.Command[card]].MovementDown[k].Movement == "attack")
							cout << "range " << Character.Deck[Character.Command[card]].MovementDown[k].range << " ";
					}
				}
				else 
				{
					for (int k = 0; k < Character.Deck[Character.Command[now_card]].MovementDown.size(); k++)
					{
						cout << Character.Deck[Character.Command[now_card]].MovementDown[k].Movement << " " << Character.Deck[Character.Command[now_card]].MovementDown[k].Movement_Value << " ";
						if (Character.Deck[Character.Command[now_card]].MovementDown[k].Movement == "attack")
							cout << "range " << Character.Deck[Character.Command[now_card]].MovementDown[k].range << " ";
					}
					if (now_card == 0)
						card = 1;
					else
						card = 0;
					cout << "	+	";
					for (int k = 0; k < Character.Deck[Character.Command[card]].MovementUp.size(); k++)
					{
						cout << Character.Deck[Character.Command[card]].MovementUp[k].Movement << " " << Character.Deck[Character.Command[card]].MovementUp[k].Movement_Value << " ";
						if (Character.Deck[Character.Command[card]].MovementUp[k].Movement == "attack")
							cout << "range " << Character.Deck[Character.Command[card]].MovementUp[k].range << " ";
					}
				}
				setPrintPosition(0, 49);
				card = now_card;
				movement = now_movement;
			}
			else 
			{
				switch (keyBoard(_getch()))
				{
				case 'w':
				case 's':
					if (now_movement == 1)
						now_movement = 0;
					else
						now_movement = 1;
					break;
				case 'a':
					check_now = true;
					now_card--;
					if (now_card <0)
						now_card = 2;
					break;
				case 'd':
					now_card++;
					check_now = true;
					if (now_card == 3)
						now_card = 0;
					break;
				case 13:
					//完成選擇
					if (now_card == 2) 
					{
						setPrintPosition(0, printPoint);
						for (int i = 0; i < 20; i++) 
						{
							cout << "                                                                                                                         " << endl;
						}
						int clean = game_Massage(play_Character, Monster, Game_Map, game_Massage_string);
						setPrintPosition(0, printPoint + clean + 3);
						system("pause");
						setPrintPosition(0, printPoint);
						for (int i = 0; i < clean + 10; i++)
						{
							cout << "                                                                                                                         " << endl;
						}
						setPrintPosition(0, 0);
						setPrintPosition(0, printPoint);
						cout << "====請選擇" << Character.ID << "的卡牌指令====" << endl;
						cout << "角色：" << Character.Character_name << "	[hp：(" << Character.Hp << "/" << Character.Max_HP << ")][shield：" << Character.Shield << "]" << endl;
						for (int i = 0; i < 2; i++)
						{
							SetColor(7);
							setPrintPosition(i * 6, printPoint + 2);	cout << "�歈�������";
							setPrintPosition(i * 6, printPoint + 3);	cout << "�� " << setw(2) << Character.Deck[Character.Command[i]].ID << "��";
							setPrintPosition(i * 6, printPoint + 4);	cout << "�� 上��";
							setPrintPosition(i * 6, printPoint + 6);	cout << "�� " << setw(2) << Character.Deck[Character.Command[i]].ID << "��";
							setPrintPosition(i * 6, printPoint + 7);	cout << "�� 下��";
							setPrintPosition(i * 6, printPoint + 8);	cout << "�裺�������";
						}
						SetColor(14);
						setPrintPosition(2 * 6, printPoint + 4);	cout << "�歈�������";
						setPrintPosition(2 * 6, printPoint + 5);	cout << "�� Ｃ��";
						setPrintPosition(2 * 6, printPoint + 6);	cout << "�裺�������";
						setPrintPosition(0, printPoint + 10);
						SetColor(7);
						cout << "                                                                                                                         " << endl;
						setPrintPosition(0, 49);
					}
					else 
					{
						setPrintPosition(0, printPoint + 2);
						for (int i = 0; i < 9; i++)
						{
							cout << "                                                                                                                         " << endl;
						}
						setPrintPosition(0, printPoint + 3);
						cout << "攻擊指令 -> ";
						if (now_movement == 0)
						{
							for (int k = 0; k < Character.Deck[Character.Command[now_card]].MovementUp.size(); k++)
							{
								cout << Character.Deck[Character.Command[now_card]].MovementUp[k].Movement << " " << Character.Deck[Character.Command[now_card]].MovementUp[k].Movement_Value << " ";
								if (Character.Deck[Character.Command[now_card]].MovementUp[k].Movement == "attack")
									cout << "range " << Character.Deck[Character.Command[now_card]].MovementUp[k].range << " ";
							}
							if (now_card == 0)
								card = 1;
							else
								card = 0;
							for (int k = 0; k < Character.Deck[Character.Command[card]].MovementDown.size(); k++)
							{
								cout << Character.Deck[Character.Command[card]].MovementDown[k].Movement << " " << Character.Deck[Character.Command[card]].MovementDown[k].Movement_Value << " ";
								if (Character.Deck[Character.Command[card]].MovementDown[k].Movement == "attack")
									cout << "range " << Character.Deck[Character.Command[card]].MovementDown[k].range << " ";
							}
						}
						else
						{
							for (int k = 0; k < Character.Deck[Character.Command[now_card]].MovementDown.size(); k++)
							{
								cout << Character.Deck[Character.Command[now_card]].MovementDown[k].Movement << " " << Character.Deck[Character.Command[now_card]].MovementDown[k].Movement_Value << " ";
								if (Character.Deck[Character.Command[now_card]].MovementDown[k].Movement == "attack")
									cout << "range " << Character.Deck[Character.Command[now_card]].MovementDown[k].range << " ";
							}
							if (now_card == 0)
								card = 1;
							else
								card = 0;
							cout << "	+	";
							for (int k = 0; k < Character.Deck[Character.Command[card]].MovementUp.size(); k++)
							{
								cout << Character.Deck[Character.Command[card]].MovementUp[k].Movement << " " << Character.Deck[Character.Command[card]].MovementUp[k].Movement_Value << " ";
								if (Character.Deck[Character.Command[card]].MovementUp[k].Movement == "attack")
									cout << "range " << Character.Deck[Character.Command[card]].MovementUp[k].range << " ";
							}
						}
						setPrintPosition(0, 49);
						if (now_card == 0)
							card = 1;
						else
							card = 0;
						if (now_movement == 0)
							movement = 1;
						else
							movement = 0;
						chooseComplete = true;
					}
					break;
				}
			}
		}
		//選擇完畢 進行攻擊
		if (now_movement == 0) 
		{
			for (int j = 0; j < Character.Deck[Character.Command[now_card]].MovementUp.size(); j++)
			{
				setPrintPosition(0, printPoint + 1);
				for (int i = 0; i < 10; i++) 
				{
					cout << "                                                                                                                         " << endl;
				}
				setPrintPosition(0, 49);
				string output_log;
				if (Character.Deck[Character.Command[now_card]].MovementUp[j].Movement == "attack") {
					if (Character.Deck[Character.Command[now_card]].MovementUp[j].range == 0)
					{
						Character.Attack_UI(Character.Deck[Character.Command[now_card]].MovementUp[j].Movement_Value, 1, Monster, Game_Map,printPoint,game_Massage_string);
						//輸出attack
					}
					else 
					{
						Character.Attack_UI(Character.Deck[Character.Command[now_card]].MovementUp[j].Movement_Value, Character.Deck[Character.Command[now_card]].MovementUp[j].range, Monster, Game_Map, printPoint, game_Massage_string);
						//輸出attack
					}
				}
				else if (Character.Deck[Character.Command[now_card]].MovementUp[j].Movement == "shield") {
					Character.Skill_shield(Character.Deck[Character.Command[now_card]].MovementUp[j].Movement_Value);
					//輸出shield
					setPrintPosition(0, printPoint + 1);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 1);
					cout << "角色：" << Character.Character_name << "	[hp：(" << Character.Hp << "/" << Character.Max_HP << ")][shield：" << Character.Shield << "]" << endl;
					output_log.push_back(Character.ID);
					output_log += "獲得了" + to_string(Character.Deck[Character.Command[now_card]].MovementUp[j].Movement_Value) + "點護甲值";
					setPrintPosition(0, printPoint + 5);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 5);
					cout << output_log << endl << endl;
					system("pause");
					setPrintPosition(0, 49);
					Sleep(500);
					game_Massage_string.push_back(output_log);
				}
				else if (Character.Deck[Character.Command[now_card]].MovementUp[j].Movement == "heal") {
					Character.Skill_heal(Character.Deck[Character.Command[now_card]].MovementUp[j].Movement_Value);
					//輸出heal
					setPrintPosition(0, printPoint + 1);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 1);
					cout << "角色：" << Character.Character_name << "	[hp：(" << Character.Hp << "/" << Character.Max_HP << ")][shield：" << Character.Shield << "]" << endl;
					output_log.push_back(Character.ID);
					output_log += "獲得了" + to_string(Character.Deck[Character.Command[now_card]].MovementUp[j].Movement_Value) + "點生命值";
					setPrintPosition(0, printPoint + 5);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 5);
					cout << output_log << endl << endl;
					system("pause");
					setPrintPosition(0, 49);
					Sleep(500);
					game_Massage_string.push_back(output_log);
				}
				else if (Character.Deck[Character.Command[now_card]].MovementUp[j].Movement == "move") {
					//輸出move
					Position old_position = Character.position;
					character_move_UI(Character, Character.Deck[Character.Command[now_card]].MovementUp[j].Movement_Value, Game_Map, play_Character, Monster, printPoint);
					Position new_position = Character.position;
					output_log.push_back(Character.ID);
					output_log += "從(" + to_string(old_position.x) + "," + to_string(old_position.y) + ")移動到(" + to_string(new_position.x) + "," + to_string(new_position.y) + ")";
					SetColor(7); setPrintPosition(0, printPoint + 5);
					for (int i = 0; i < 9; i++)
					{
						cout << "                                                                                                                         " << endl;
					}
					setPrintPosition(0, printPoint + 5);
					cout << output_log << endl << endl;
					system("pause");
					SetColor(7); setPrintPosition(0, 49);
					Sleep(500);
					game_Massage_string.push_back(output_log);
				}
			}
			
			for (int j = 0; j < Character.Deck[Character.Command[card]].MovementDown.size(); j++)
			{
				setPrintPosition(0, printPoint + 1);
				for (int i = 0; i < 10; i++)
				{
					cout << "                                                                                                                         " << endl;
				}
				setPrintPosition(0, 49);
				string output_log;
				if (Character.Deck[Character.Command[card]].MovementDown[j].Movement == "attack") {
					if (Character.Deck[Character.Command[card]].MovementUp[j].range == 0)
					{
						Character.Attack_UI(Character.Deck[Character.Command[now_card]].MovementUp[j].Movement_Value, 1, Monster, Game_Map, printPoint, game_Massage_string);
						//輸出attack
					}
					else
					{
						Character.Attack_UI(Character.Deck[Character.Command[now_card]].MovementUp[j].Movement_Value, Character.Deck[Character.Command[now_card]].MovementUp[j].range, Monster, Game_Map, printPoint, game_Massage_string);
						//輸出attack
					}
				}
				else if (Character.Deck[Character.Command[card]].MovementDown[j].Movement == "shield") {
					Character.Skill_shield(Character.Deck[Character.Command[card]].MovementDown[j].Movement_Value);
					//輸出shield
					setPrintPosition(0, printPoint + 1);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 1);
					cout << "角色：" << Character.Character_name << "	[hp：(" << Character.Hp << "/" << Character.Max_HP << ")][shield：" << Character.Shield << "]" << endl;
					output_log.push_back(Character.ID);
					output_log += "獲得了" + to_string(Character.Deck[Character.Command[card]].MovementDown[j].Movement_Value) + "點護甲值";
					setPrintPosition(0, printPoint + 5);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 5);
					cout << output_log << endl << endl;
					system("pause");
					setPrintPosition(0, 49);
					Sleep(500);
					game_Massage_string.push_back(output_log);
				}
				else if (Character.Deck[Character.Command[card]].MovementDown[j].Movement == "heal") {
					Character.Skill_heal(Character.Deck[Character.Command[card]].MovementDown[j].Movement_Value);
					//輸出heal
					setPrintPosition(0, printPoint + 1);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 1);
					cout << "角色：" << Character.Character_name << "	[hp：(" << Character.Hp << "/" << Character.Max_HP << ")][shield：" << Character.Shield << "]" << endl;
					output_log.push_back(Character.ID);
					output_log += "獲得了" + to_string(Character.Deck[Character.Command[card]].MovementDown[j].Movement_Value) + "點生命值";
					setPrintPosition(0, printPoint + 5);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 5);
					cout << output_log << endl << endl;
					system("pause");
					setPrintPosition(0, 49);
					Sleep(500);
					game_Massage_string.push_back(output_log);
				}
				else if (Character.Deck[Character.Command[card]].MovementDown[j].Movement == "move") {
					//輸出move
					Position old_position = Character.position;
					character_move_UI(Character, Character.Deck[Character.Command[card]].MovementDown[j].Movement_Value, Game_Map, play_Character, Monster, printPoint);
					Position new_position = Character.position;
					output_log.push_back(Character.ID);
					output_log += "從(" + to_string(old_position.x) + "," + to_string(old_position.y) + ")移動到(" + to_string(new_position.x) + "," + to_string(new_position.y) + ")";
					SetColor(7); setPrintPosition(0, printPoint + 5);
					for (int i = 0; i < 9; i++)
					{
						cout << "                                                                                                                         " << endl;
					}
					setPrintPosition(0, printPoint + 5);
					cout << output_log << endl << endl;
					system("pause");
					SetColor(7); setPrintPosition(0, 49);
					Sleep(500);
					game_Massage_string.push_back(output_log);
				}
			}
		}
		else 
		{
			for (int j = 0; j < Character.Deck[Character.Command[now_card]].MovementDown.size(); j++)
			{
				setPrintPosition(0, printPoint + 1);
				for (int i = 0; i < 10; i++)
				{
					cout << "                                                                                                                         " << endl;
				}
				setPrintPosition(0, 49);
				string output_log;
				if (Character.Deck[Character.Command[now_card]].MovementDown[j].Movement == "attack") {
					if (Character.Deck[Character.Command[now_card]].MovementUp[j].range == 0)
					{
						Character.Attack_UI(Character.Deck[Character.Command[now_card]].MovementUp[j].Movement_Value, 1, Monster, Game_Map, printPoint, game_Massage_string);
						//輸出attack
					}
					else
					{
						Character.Attack_UI(Character.Deck[Character.Command[now_card]].MovementUp[j].Movement_Value, Character.Deck[Character.Command[now_card]].MovementUp[j].range, Monster, Game_Map, printPoint, game_Massage_string);
						//輸出attack
					}
				}
				else if (Character.Deck[Character.Command[now_card]].MovementDown[j].Movement == "shield") {
					Character.Skill_shield(Character.Deck[Character.Command[now_card]].MovementDown[j].Movement_Value);
					//輸出shield
					setPrintPosition(0, printPoint + 1);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 1);
					cout << "角色：" << Character.Character_name << "	[hp：(" << Character.Hp << "/" << Character.Max_HP << ")][shield：" << Character.Shield << "]" << endl;
					output_log.push_back(Character.ID);
					output_log += "獲得了" + to_string(Character.Deck[Character.Command[now_card]].MovementDown[j].Movement_Value) + "點護甲值";
					setPrintPosition(0, printPoint + 5);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 5);
					cout << output_log << endl << endl;
					system("pause");
					setPrintPosition(0, 49);
					game_Massage_string.push_back(output_log);
				}
				else if (Character.Deck[Character.Command[now_card]].MovementDown[j].Movement == "heal") {
					Character.Skill_heal(Character.Deck[Character.Command[now_card]].MovementDown[j].Movement_Value);
					//輸出heal
					setPrintPosition(0, printPoint + 1);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 1);
					cout << "角色：" << Character.Character_name << "	[hp：(" << Character.Hp << "/" << Character.Max_HP << ")][shield：" << Character.Shield << "]" << endl;
					output_log.push_back(Character.ID);
					output_log += "獲得了" + to_string(Character.Deck[Character.Command[now_card]].MovementDown[j].Movement_Value) + "點生命值";
					setPrintPosition(0, printPoint + 5);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 5);
					cout << output_log << endl << endl;
					system("pause");
					setPrintPosition(0, 49);
					game_Massage_string.push_back(output_log);
				}
				else if (Character.Deck[Character.Command[now_card]].MovementDown[j].Movement == "move") {
					//輸出move
					Position old_position = Character.position;
					character_move_UI(Character, Character.Deck[Character.Command[now_card]].MovementDown[j].Movement_Value, Game_Map, play_Character, Monster, printPoint);
					Position new_position = Character.position;
					output_log.push_back(Character.ID);
					output_log += "從(" + to_string(old_position.x) + "," + to_string(old_position.y) + ")移動到(" + to_string(new_position.x) + "," + to_string(new_position.y) + ")";
					SetColor(7); setPrintPosition(0, printPoint + 5);
					for (int i = 0; i < 9; i++)
					{
						cout << "                                                                                                                         " << endl;
					}
					setPrintPosition(0, printPoint + 5);
					cout << output_log << endl << endl;
					system("pause");
					SetColor(7); setPrintPosition(0, 49);
					game_Massage_string.push_back(output_log);
				}
			}

			for (int j = 0; j < Character.Deck[Character.Command[card]].MovementUp.size(); j++)
			{
				setPrintPosition(0, printPoint + 1);
				for (int i = 0; i < 10; i++)
				{
					cout << "                                                                                                                         " << endl;
				}
				setPrintPosition(0, 49);
				string output_log;
				if (Character.Deck[Character.Command[card]].MovementUp[j].Movement == "attack") {
					if (Character.Deck[Character.Command[card]].MovementUp[j].range == 0)
					{
						Character.Attack_UI(Character.Deck[Character.Command[card]].MovementUp[j].Movement_Value, 1, Monster, Game_Map, printPoint, game_Massage_string);
						//輸出attack
					}
					else
					{
						Character.Attack_UI(Character.Deck[Character.Command[card]].MovementUp[j].Movement_Value, Character.Deck[Character.Command[card]].MovementUp[j].range, Monster, Game_Map, printPoint, game_Massage_string);
						//輸出attack
					}
				}
				else if (Character.Deck[Character.Command[card]].MovementUp[j].Movement == "shield") {
					Character.Skill_shield(Character.Deck[Character.Command[card]].MovementUp[j].Movement_Value);
					//輸出shield
					setPrintPosition(0, printPoint + 1);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 1);
					cout << "角色：" << Character.Character_name << "	[hp：(" << Character.Hp << "/" << Character.Max_HP << ")][shield：" << Character.Shield << "]" << endl;
					output_log.push_back(Character.ID);
					output_log += "獲得了" + to_string(Character.Deck[Character.Command[card]].MovementUp[j].Movement_Value) + "點護甲值";
					setPrintPosition(0, printPoint + 5);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 5);
					cout << output_log << endl << endl;
					system("pause");
					setPrintPosition(0, 49);
					game_Massage_string.push_back(output_log);
				}
				else if (Character.Deck[Character.Command[card]].MovementUp[j].Movement == "heal") {
					Character.Skill_heal(Character.Deck[Character.Command[card]].MovementUp[j].Movement_Value);
					//輸出heal
					setPrintPosition(0, printPoint + 1);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 1);
					cout << "角色：" << Character.Character_name << "	[hp：(" << Character.Hp << "/" << Character.Max_HP << ")][shield：" << Character.Shield << "]" << endl;
					output_log.push_back(Character.ID);
					output_log += "獲得了" + to_string(Character.Deck[Character.Command[card]].MovementUp[j].Movement_Value) + "點生命值";
					setPrintPosition(0, printPoint + 5);
					cout << "                                                                                                                         " << endl;
					setPrintPosition(0, printPoint + 5);
					cout << output_log << endl << endl;
					system("pause");
					setPrintPosition(0, 49);
					game_Massage_string.push_back(output_log);
				}
				else if (Character.Deck[Character.Command[card]].MovementUp[j].Movement == "move") {
					//輸出move
					Position old_position = Character.position;
					character_move_UI(Character, Character.Deck[Character.Command[card]].MovementUp[j].Movement_Value, Game_Map, play_Character, Monster, printPoint);
					Position new_position = Character.position;
					output_log.push_back(Character.ID);
					output_log += "從(" + to_string(old_position.x) + "," + to_string(old_position.y) + ")移動到(" + to_string(new_position.x) + "," + to_string(new_position.y) + ")";
					SetColor(7); setPrintPosition(0, printPoint + 5);
					for (int i = 0; i < 9; i++)
					{
						cout << "                                                                                                                         " << endl;
					}
					setPrintPosition(0, printPoint + 5);
					cout << output_log << endl << endl;
					system("pause");
					SetColor(7); setPrintPosition(0, 49);
					Sleep(500);
					game_Massage_string.push_back(output_log);
				}
			}
		}

		Character.Deck[Character.Command[0]].status = 2;
		Character.Deck[Character.Command[1]].status = 2;
	}
	else
	{
		cout << "====長休====" << endl << endl;
		cout << "請選擇要丟棄的牌";
		string output_log = "";
		vector<int> can_throw_Card;
		for (int i = 0; i < Character.Deck.size(); i++)
		{
			if (Character.Deck[i].status == 2)
				can_throw_Card.push_back(i);
		}
		for (int i = 0; i < can_throw_Card.size(); i++)
		{
			SetColor(7);
			setPrintPosition(i * 5, printPoint + 4);	cout << "�歈�����";
			setPrintPosition(i * 5, printPoint + 5);	cout << "��" << setw(2) << Character.Deck[can_throw_Card[i]].ID << "��";
			setPrintPosition(i * 5, printPoint + 6);	cout << "�裺�����";
		}
		SetColor(7); setPrintPosition(0, printPoint + 10);
		cout << "※ Enter：確認";
		bool checkComplete = false;
		int nowCard = 0, card = 1;
		setPrintPosition(0, 49);
		while (checkComplete == false)
		{
			if (nowCard != card)
			{
				SetColor(7);
				setPrintPosition(card * 5, printPoint + 4);	cout << "�歈�����";
				setPrintPosition(card * 5, printPoint + 5);	cout << "��" << setw(2) << can_throw_Card[card] << "��";
				setPrintPosition(card * 5, printPoint + 6);	cout << "�裺�����";
				SetColor(14);
				setPrintPosition(nowCard * 5, printPoint + 4);	cout << "�歈�����";
				setPrintPosition(nowCard * 5, printPoint + 5);	cout << "��" << setw(2) << can_throw_Card[nowCard] << "��";
				setPrintPosition(nowCard * 5, printPoint + 6);	cout << "�裺�����";
				setPrintPosition(0, printPoint + 8);
				SetColor(7);
				cout << "                                                                                                                         " << endl;
				setPrintPosition(0, printPoint + 8);
				cout << "編號：	" << Character.Deck[nowCard].ID << "	敏捷值：" << Character.Deck[nowCard].Dexterity_Value << "	上：";
				for (int k = 0; k < Character.Deck[nowCard].MovementUp.size(); k++)
				{
					cout << Character.Deck[nowCard].MovementUp[k].Movement << " " << Character.Deck[nowCard].MovementUp[k].Movement_Value << " ";
					if (Character.Deck[nowCard].MovementUp[k].Movement == "attack")
						cout << "range " << Character.Deck[nowCard].MovementUp[k].range << " ";
				}
				cout << " | 下：";
				for (int k = 0; k < Character.Deck[nowCard].MovementDown.size(); k++)
				{
					cout << Character.Deck[nowCard].MovementDown[k].Movement << " " << Character.Deck[nowCard].MovementDown[k].Movement_Value << " ";
					if (Character.Deck[nowCard].MovementDown[k].Movement == "attack")
						cout << "range " << Character.Deck[nowCard].MovementDown[k].range << " ";
				}
				setPrintPosition(0, 49);
				card = nowCard;
			}
			else
			{
				switch (keyBoard(_getch()))
				{
				case 'a':
					nowCard--;
					if (nowCard < 0)
						nowCard = can_throw_Card.size() - 1;
					break;
				case 'd':
					nowCard++;
					if (nowCard == can_throw_Card.size())
						nowCard = 0;
					break;
				case 13:
					//完成選擇
					output_log.push_back(Character.ID);
					output_log += " 本輪長休，丟棄" + to_string(can_throw_Card[nowCard]) + "號卡，回復2點生命值";
					SetColor(7); setPrintPosition(0, printPoint + 2);
					for (int i = 0; i < 10; i++)
					{
						cout << "                                                                                                                         " << endl;
					}
					SetColor(7); setPrintPosition(0, printPoint + 2);
					cout << output_log;
					game_Massage_string.push_back(output_log);
					Sleep(500);
					SetColor(7); setPrintPosition(0, 49);
					Character.Deck[can_throw_Card[nowCard]].status = 3;
					for (int i = 0; i < Character.Deck.size(); i++) 
					{
						if (Character.Deck[i].status == 2)
							Character.Deck[i].status = 1;
					}
					Character.Skill_heal(2);
					checkComplete = true;
					break;
				}
			}
		}
	}
	SetColor(7); setPrintPosition(0, printPoint);
	for (int i = 0; i < 10; i++)
	{
		cout << "                                                                                                                         " << endl;
	}
}
void character_move_UI(Character& C, int step, Map& Game_Map, vector<Character> play_Character, vector<Ethnicity> Monster, int printPoint)
{
	setPrintPosition(0, printPoint + 5);
	cout << "還有" << step << "步" << endl << endl;
	cout << "※使用wasd移動角色" << C.ID;	//7
	setPrintPosition(0, 49);
	vector<Position> stephistory;
	bool chooseComplete = false;
	while (chooseComplete == false)
	{
		SetColor(7); setPrintPosition(0, printPoint + 5);
		cout << "還有" << step << "步" << endl << endl;
		setPrintPosition(0, 49);
		switch (keyBoard(_getch()))
		{
		case 'w':
			if (step == 0)
			{
				if (stephistory[stephistory.size() - 1].y != C.position.y - 1)
					break;
			}
			if ((Game_Map.Game_Map[C.position.y - 1][C.position.x] == 1 || Game_Map.Game_Map[C.position.y - 1][C.position.x] == 3) && samePositionMonster(C.position.x, C.position.y - 1, Monster) == false)
			{
				if (stephistory.size() == 0)
				{
					step--;
					setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(176);
					if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
					{
						if (Game_Map.Game_Map[C.position.y][C.position.x] == 3)
						{
							cout << "∩";
						}
						else
							cout << "  ";
					}
					else
					{
						cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
					}
					Position newhistory;
					newhistory.x = C.position.x; newhistory.y = C.position.y;
					stephistory.push_back(newhistory);
				}
				else if (C.position.y - 1 == stephistory[stephistory.size() - 1].y && C.position.x == stephistory[stephistory.size() - 1].x)
				{
					stephistory.pop_back();
					step++;
					setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(7);
					if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
					{
						if (Game_Map.Game_Map[C.position.y][C.position.x] == 3)
						{
							cout << "∩";
						}
						else
							cout << "□";
					}
					else
					{
						SetColor(10);	cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
					}
				}
				else
				{
					step--;
					setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(176);
					if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
					{
						if (Game_Map.Game_Map[C.position.y][C.position.x] == 3)
						{
							cout << "∩";
						}
						else
							cout << "  ";
					}
					else
					{
						cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
					}
					Position newhistory;
					newhistory.x = C.position.x; newhistory.y = C.position.y;
					stephistory.push_back(newhistory);
				}
				C.position.y--;
				setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(7);
				if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
				{
					SetColor(10); cout << "p" << C.ID;
				}
				else
				{
					SetColor(202);	cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
				}

			}
			SetColor(7);	setPrintPosition(0, 49);
			break;
		case 's':
			if (step == 0)
			{
				if (stephistory[stephistory.size() - 1].y != C.position.y + 1)
					break;
			}
			if ((Game_Map.Game_Map[C.position.y + 1][C.position.x] == 1 || Game_Map.Game_Map[C.position.y + 1][C.position.x] == 3) && samePositionMonster(C.position.x, C.position.y + 1, Monster) == false)
			{
				if (stephistory.size() == 0)
				{
					step--;
					setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(176);
					if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
					{
						if (Game_Map.Game_Map[C.position.y][C.position.x] == 3)
						{
							cout << "∩";
						}
						else
							cout << "  ";
					}
					else
					{
						cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
					}
					Position newhistory;
					newhistory.x = C.position.x; newhistory.y = C.position.y;
					stephistory.push_back(newhistory);
				}
				else if (C.position.y + 1 == stephistory[stephistory.size() - 1].y && C.position.x == stephistory[stephistory.size() - 1].x)
				{
					stephistory.pop_back();
					step++;
					setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(7);
					if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
					{
						if (Game_Map.Game_Map[C.position.y][C.position.x] == 3)
						{
							cout << "∩";
						}
						else
							cout << "□";
					}
					else
					{
						SetColor(10);	cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
					}
				}
				else
				{
					step--;
					setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(176);
					if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
					{
						if (Game_Map.Game_Map[C.position.y][C.position.x] == 3)
						{
							cout << "∩";
						}
						else
							cout << "  ";
					}
					else
					{
						cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
					}
					Position newhistory;
					newhistory.x = C.position.x; newhistory.y = C.position.y;
					stephistory.push_back(newhistory);
				}
				C.position.y++;
				setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(7);
				if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
				{
					SetColor(10); cout << "p" << C.ID;
				}
				else
				{
					SetColor(202);	cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
				}
			}
			SetColor(7);	setPrintPosition(0, 49);
			break;
		case 'a':
			if (step == 0)
			{
				if (stephistory[stephistory.size() - 1].x != C.position.x - 1)
					break;
			}
			if ((Game_Map.Game_Map[C.position.y][C.position.x - 1] == 1 || Game_Map.Game_Map[C.position.y][C.position.x - 1] == 3) && samePositionMonster(C.position.x - 1, C.position.y, Monster) == false)
			{
				if (stephistory.size() == 0)
				{
					step--;
					setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(176);
					if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
					{
						if (Game_Map.Game_Map[C.position.y][C.position.x] == 3)
						{
							cout << "∩";
						}
						else
							cout << "  ";
					}
					else
					{
						cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
					}
					Position newhistory;
					newhistory.x = C.position.x; newhistory.y = C.position.y;
					stephistory.push_back(newhistory);
				}
				else if (C.position.y == stephistory[stephistory.size() - 1].y && C.position.x - 1 == stephistory[stephistory.size() - 1].x)
				{
					stephistory.pop_back();
					step++;
					setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(7);
					if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
					{
						if (Game_Map.Game_Map[C.position.y][C.position.x] == 3)
						{
							cout << "∩";
						}
						else
							cout << "□";
					}
					else
					{
						SetColor(10);	cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
					}
				}
				else
				{
					step--;
					setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(176);
					if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
					{
						if (Game_Map.Game_Map[C.position.y][C.position.x] == 3)
						{
							cout << "∩";
						}
						else
							cout << "  ";
					}
					else
					{
						cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
					}
					Position newhistory;
					newhistory.x = C.position.x; newhistory.y = C.position.y;
					stephistory.push_back(newhistory);
				}
				C.position.x--;
				setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(7);
				if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
				{
					SetColor(10); cout << "p" << C.ID;
				}
				else
				{
					SetColor(202);	cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
				}
			}
			SetColor(7);	setPrintPosition(0, 49);
			break;
		case 'd':
			if (step == 0)
			{
				if (stephistory[stephistory.size() - 1].x != C.position.x + 1)
					break;
			}
			if ((Game_Map.Game_Map[C.position.y][C.position.x + 1] == 1 || Game_Map.Game_Map[C.position.y][C.position.x + 1] == 3) && samePositionMonster(C.position.x + 1, C.position.y, Monster) == false)
			{
				if (stephistory.size() == 0)
				{
					step--;
					setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(176);
					if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
					{
						if (Game_Map.Game_Map[C.position.y][C.position.x] == 3)
						{
							cout << "∩";
						}
						else
							cout << "  ";
					}
					else
					{
						cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
					}
					Position newhistory;
					newhistory.x = C.position.x; newhistory.y = C.position.y;
					stephistory.push_back(newhistory);
				}
				else if (C.position.y == stephistory[stephistory.size() - 1].y && C.position.x + 1 == stephistory[stephistory.size() - 1].x)
				{
					stephistory.pop_back();
					step++;
					setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(7);
					if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
					{
						if (Game_Map.Game_Map[C.position.y][C.position.x] == 3)
						{
							cout << "∩";
						}
						else
							cout << "□";
					}
					else
					{
						SetColor(10);	cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
					}
				}
				else
				{
					step--;
					setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(176);
					if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
					{
						if (Game_Map.Game_Map[C.position.y][C.position.x] == 3)
						{
							cout << "∩";
						}
						else
							cout << "  ";
					}
					else
					{
						cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
					}
					Position newhistory;
					newhistory.x = C.position.x; newhistory.y = C.position.y;
					stephistory.push_back(newhistory);
				}
				C.position.x++;
				setPrintPosition((C.position.x + 1) * 2, C.position.y); SetColor(7);
				if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
				{
					SetColor(10); cout << "p" << C.ID;
				}
				else
				{
					SetColor(202);	cout << "p" << play_Character[samePositionCharacter(C.position.x, C.position.y, C, play_Character)].ID;
				}
			}
			SetColor(7);	setPrintPosition(0, 49);
			break;
		case 13:
			if (samePositionCharacter(C.position.x, C.position.y, C, play_Character) == -1)
			{
				chooseComplete = true;
				for (int i = 0; i < stephistory.size(); i++)
				{
					SetColor(7); setPrintPosition((stephistory[i].x + 1) * 2, stephistory[i].y);
					if (samePositionCharacter(stephistory[i].x, stephistory[i].y, C, play_Character) == -1)
					{
						if (Game_Map.Game_Map[stephistory[i].y][stephistory[i].x] == 3)
						{
							cout << "∩";
						}
						else
							cout << "□";
					}
					else
					{
						SetColor(10);	cout << "p" << play_Character[samePositionCharacter(stephistory[i].x, stephistory[i].y, C, play_Character)].ID;
					}
					setPrintPosition(0, 49);
					Sleep(100);
				}
			}

			break;
		}
	}
}
int samePositionCharacter(int x, int y, Character C, vector<Character> play_Character)
{
	for (int i = 0; i < play_Character.size(); i++)
	{
		if (play_Character[i].position.x == x && play_Character[i].position.y == y && play_Character[i].ID != C.ID)
			return i;
	}
	return -1;
}
bool samePositionMonster(int x, int y, vector<Ethnicity> Monster) 
{
	for (int i = 0; i < Monster.size(); i++) 
	{
		for (int j = 0; j < Monster[i].Creature_List.size(); j++) 
		{
			if (Monster[i].Creature_List[j].position.x == x && Monster[i].Creature_List[j].position.y == y)
				return true;
		}
	}
	return false;
}
void monsters_round_UI(vector<Character>& play_Character, Ethnicity& Monster_Ethnicity, Monster_Base& monster, Map Game_Map, vector<Ethnicity>& Monster, vector<char> attack_Sort, int printPoint, vector<string>& game_Massage_string) //怪物回合
{
	setPrintPosition(0, printPoint);
	for (int i = 0; i < 20; i++)
	{
		cout << "                                                                                                                         " << endl;
	}
	if (Monster_Ethnicity.Command == -1) 
	{
		return;
	}
	for (int i = 0; i < Monster_Ethnicity.Deck.size(); i++)
	{
		setPrintPosition(0, printPoint);
		cout << "====" << Monster_Ethnicity.Ethnicity_Name << "行動階段====" << endl << endl;
		if (Monster_Ethnicity.Deck[i].status == 2)
		{
			for (int j = 0; j < Monster_Ethnicity.Deck[i].Movement.size(); j++)
			{
				string output_log;
				setPrintPosition(0, printPoint + 2);
				for (int i = 0; i < 10; i++)
				{
					cout << "                                                                                                                         " << endl;
				}
				setPrintPosition(0, printPoint + 2);
				if (Monster_Ethnicity.Deck[i].Movement[j].Movement == "attack") {
					monster_Attack_UI(monster, Monster_Ethnicity.Deck[i].Movement[j].Movement_Value, Monster_Ethnicity.Deck[i].Movement[j].range, attack_Sort, Monster, play_Character, Game_Map, printPoint, game_Massage_string);
				}
				else if (Monster_Ethnicity.Deck[i].Movement[j].Movement == "shield") {
					monster.Skill_shield(Monster_Ethnicity.Deck[Monster_Ethnicity.Command].Movement[j].Movement_Value);
					output_log.push_back(monster.icon);
					output_log += "獲得了" + to_string(Monster_Ethnicity.Deck[Monster_Ethnicity.Command].Movement[j].Movement_Value) + "點護甲值";
					setPrintPosition(0, printPoint + 5);
					cout << output_log << endl;
					setPrintPosition(0, 49);
					Sleep(500);
					game_Massage_string.push_back(output_log);
				}
				else if (Monster_Ethnicity.Deck[i].Movement[j].Movement == "heal") {
					monster.Skill_heal(Monster_Ethnicity.Deck[Monster_Ethnicity.Command].Movement[j].Movement_Value);
					output_log.push_back(monster.icon);
					output_log += "獲得了" + to_string(Monster_Ethnicity.Deck[Monster_Ethnicity.Command].Movement[j].Movement_Value) + "點生命值";
					setPrintPosition(0, printPoint + 5);
					cout << output_log << endl;
					setPrintPosition(0, 49);
					Sleep(500);
					game_Massage_string.push_back(output_log);
				}
				else if (Monster_Ethnicity.Deck[i].Movement[j].Movement == "move") {
					Position old_position = monster.position;
					setPrintPosition((old_position.x + 1) * 2, old_position.y);
					cout << "□";
					monster_move(monster, Monster_Ethnicity.Deck[i].Movement[j].Move_Command, Game_Map, play_Character, Monster);
					Position new_position = monster.position;
					setPrintPosition((new_position.x + 1) * 2, new_position.y);
					SetColor(12); cout << monster.icon;
					if (monster.mode == 2)
						cout << "+";
					else
						cout << "-";
					SetColor(7);
					output_log.push_back(monster.icon);
					output_log += "從(" + to_string(old_position.x) + "," + to_string(old_position.y) + ")移動到(" + to_string(new_position.x) + "," + to_string(new_position.y) + ")";
					setPrintPosition(0, printPoint + 5);
					cout << output_log << endl;
					SetColor(7); setPrintPosition(0, 49);
					Sleep(500);
					game_Massage_string.push_back(output_log);
				}
			}
			break;
			Monster_Ethnicity.Deck[i].status == 1;
		}
	}
	setPrintPosition(0, printPoint);
	for (int i = 0; i < 20; i++)
	{
		cout << "                                                                                                                         " << endl;
	}
}
void monster_Attack_UI(Monster_Base& M, int value, int range, vector<char> attack_Sort, vector<Ethnicity> Monster, vector<Character>& play_Character, Map& Game_Map, int printPoint, vector<string>& game_Massage_string) 
{
	vector<int> target_List;
	int min_Distance = -1;
	for (int i = 0; i < play_Character.size(); i++)
	{
		if (min_Distance == -1)
			min_Distance = abs(M.position.x - play_Character[i].position.x) + abs(M.position.y - play_Character[i].position.y);
		int distance = abs(M.position.x - play_Character[i].position.x) + abs(M.position.y - play_Character[i].position.y);
		if (range + M.Range == 0)
		{
			if (distance == 1)
			{
				target_List.push_back(i);
			}
		}
		else
		{
			if (distance <= range + M.Range && vision_search(M.position, play_Character[i].position, Game_Map) == false)
			{
				target_List.push_back(i);
			}
		}
		if (distance < min_Distance)
			min_Distance = distance;
	}
	for (int i = target_List.size() - 1; i >= 0; i--)
	{
		if ((abs(M.position.x - play_Character[target_List[i]].position.x) + abs(M.position.y - play_Character[target_List[i]].position.y)) > min_Distance)
			target_List.erase(target_List.begin() + i);
	}
	int final_Target = 99;
	for (int i = 0; i < attack_Sort.size(); i++)
	{
		for (int j = 0; j < target_List.size(); j++)
		{
			if (play_Character[target_List[j]].ID == attack_Sort[i])
			{
				final_Target = target_List[j];
			}
		}
		if (final_Target != 99)
			break;
	}
	string output_log = "";
	if (final_Target == 99)
	{
		output_log.push_back(M.icon);
		output_log += "放棄攻擊";
		setPrintPosition(0, printPoint + 5);
		cout << output_log << endl;
		SetColor(7); setPrintPosition(0, 49);
		Sleep(500);
		game_Massage_string.push_back(output_log);
		return;
	}
	int distance = abs(M.position.x - play_Character[final_Target].position.x) + abs(M.position.y - play_Character[final_Target].position.y);
	setPrintPosition(0, printPoint + 5); cout << M.icon << " 鎖定 " << play_Character[final_Target].ID << " 距離：" << distance;
	//b attack A 3 damage, A shield 1, A remain 12 hp
	output_log.push_back(M.icon); output_log += " 攻擊 "; output_log.push_back(play_Character[final_Target].ID); output_log += "(護甲：" + to_string(play_Character[final_Target].Shield) + ") " + to_string(M.Damage + value) + "點傷害";
	if (play_Character[final_Target].Shield < M.Damage + value)
	{
		play_Character[final_Target].Hp -= ((M.Damage + value) - play_Character[final_Target].Shield);
	}
	else
	{
		//play_Character[final_Target].Shield -= (M.Damage + value);
	}
	if (play_Character[final_Target].Hp <= 0)
	{
		output_log.push_back(play_Character[final_Target].ID); output_log += " 死亡";
		setPrintPosition((play_Character[final_Target].position.x + 1) * 2, play_Character[final_Target].position.y);
		cout << "□";
		setPrintPosition(0, 49);
		play_Character.erase(play_Character.begin() + final_Target);
	}
	else 
	{
		output_log.push_back(play_Character[final_Target].ID); output_log += " 剩餘 " + to_string(play_Character[final_Target].Hp) + "點生命值";
	}
	setPrintPosition(0, printPoint + 7);
	cout << output_log << endl;
	SetColor(7); setPrintPosition(0, 49);
	Sleep(500);
	game_Massage_string.push_back(output_log);
	return;
}
void end_round_UI(vector<Character>& play_Character, vector<Ethnicity>& Monster, Map& Map) 
{
	bool no_monster = true;
	//護甲歸0
	for (int i = 0; i < play_Character.size(); i++) {
		play_Character[i].Shield = 0;
		play_Character[i].Rest = false;
	}
	for (int i = 0; i < Monster.size(); i++) {
		if (Monster[i].Command != -1)
		{
			Monster[i].Deck[Monster[i].Command].status = 1;
		}
		for (int j = 0; j < Monster[i].Creature_List.size(); j++) {
			Monster[i].Creature_List[j].Shield = 0;
			if (Monster[i].Creature_List[j].active) {
				no_monster = false;
			}
		}
		//怪物判斷是否有重洗
		if (Monster[i].Shuffle_Mark) {
			for (int j = 0; j < Monster[i].Deck.size(); j++) {
				if (Monster[i].Deck[j].status == 1) {
					Monster[i].Deck[j].status = 0;
				}
			}
			Monster[i].Shuffle_Mark = false;
		}
	}
	//判斷角色有沒有在門上和判斷有沒有剩餘怪物 開門 and 重置角色長休狀態
	bool open_Door = false;
	for (int i = 0; i < play_Character.size(); i++)
	{
		play_Character[i].Rest = false;
		if (Map.Game_Map[play_Character[i].position.y][play_Character[i].position.x] == 3 && no_monster == true) {
			Map.Game_Map[play_Character[i].position.y][play_Character[i].position.x] = 1;
			open_Door = true;
		}
	}
	//等所有門都開完再重新探視野

	if (open_Door == true)
	{
		for (int i = 0; i < play_Character.size(); i++)
		{
			Map.check_road(play_Character[i].position.x, play_Character[i].position.y);
		}
		Map.print_Map_UI(play_Character, Monster);
	}

	for (int i = 0; i < Monster.size(); i++)
	{
		for (int j = 0; j < Monster[i].Creature_List.size(); j++)
		{
			int x = Monster[i].Creature_List[j].position.x;
			int y = Monster[i].Creature_List[j].position.y;
			if (Map.Game_Map[y][x] == 1)
			{
				Monster[i].Creature_List[j].active = true;
			}
		}
	}
	for (int i = play_Character.size() - 1; i >= 0; i--)
	{
		int can_use_card = 0, can_throw_card = 0;
		for (int j = 0; j < play_Character[i].Deck.size(); j++) 
		{
			if (play_Character[i].Deck[j].status == 1)
				can_use_card++;
			if (play_Character[i].Deck[j].status == 2)
				can_throw_card++;
		}
		if (can_use_card < 2 && can_throw_card <2) 
		{
			setPrintPosition((play_Character[i].position.x + 1) * 2, play_Character[i].position.y);
			cout << "□";
			setPrintPosition(0, 49);
			play_Character.erase(play_Character.begin() + i);
		}
	}
}
bool end_Game_UI(vector<Character>& play_Character, vector<Ethnicity>& Monster, Map& Map, bool UI_mode) 
{
	for (int i = play_Character.size() - 1; i >= 0; i--)
	{
		int can_use_card = 0, can_throw_card = 0;
		for (int j = 0; j < play_Character[i].Deck.size(); j++)
		{
			if (play_Character[i].Deck[j].status == 1)
				can_use_card++;
			if (play_Character[i].Deck[j].status == 2)
				can_throw_card++;
		}
		if (can_use_card < 2 && can_throw_card < 2)
		{
			setPrintPosition((play_Character[i].position.x + 1) * 2, play_Character[i].position.y);
			SetColor(7); cout << "□";
			setPrintPosition(0, 49);
			play_Character.erase(play_Character.begin() + i);
		}
	}
	if (play_Character.size() == 0)
	{
		if (UI_mode == false)
			cout << "monster win~" << endl;
		else
		{
			system("cls");
			cout << "monster win~" << endl;
		}
		return false;
	}
	bool all_Monsters_Dead = true;
	for (int i = 0; i < Monster.size(); i++)
	{
		if (Monster[i].Creature_List.size() != 0)
		{
			all_Monsters_Dead = false;
		}
	}
	bool all_Door_open = true;
	for (int i = 0; i < Map.High; i++)
	{
		for (int j = 0; j < Map.Width; j++)
		{
			if (Map.Game_Map[i][j] == 3)
			{
				all_Door_open = false;
			}
		}
	}
	if (all_Monsters_Dead == true && all_Door_open == true)
	{
		if (UI_mode == false)
			cout << "character win~" << endl;
		else
		{
			system("cls");
			cout << "character win~" << endl;
		}
		return false;
	}
	return true;
}
char keyBoard(char input) 
{
	switch (input) 
	{
	case 'w':
	case 'W':
	case 72:
		return 'w';
		break;
	case 'a':
	case 'A':
	case 75:
		return 'a';
		break;
	case 's':
	case 'S':
	case 80:
		return 's';
		break;
	case 'd':
	case 'D':
	case 77:
		return 'd';
		break;
	case 13:
		return 13;
		break;
	case 27:
		return 27;
		break;
	}
}
/*=============Windows.h的function==============*/
void setPrintPosition(int x, int y)		//改變Print的起始位置
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coordinate;
	coordinate.X = x;
	coordinate.Y = y;
	SetConsoleCursorPosition(out, coordinate);
}
void SetColor(int color)				//改變顏色
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}
void ConsoleFullScreen()
{
	keybd_event(VK_MENU, 0x38, 0, 0);
	keybd_event(VK_RETURN, 0x1c, 0, 0);
	keybd_event(VK_MENU, 0xb8, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_RETURN, 0x9c, KEYEVENTF_KEYUP, 0);
}