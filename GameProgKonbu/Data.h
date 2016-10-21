#pragma once
#include "useful_func_and_class.h"

class Score final
{
public:
	uint32_t use_memory;
	uint32_t use_time;
	enum class Type_T{ AC,WA,TLE,MLE,RE }type;
	TCHAR input_name[10];//���̓t�@�C���̖��O
};
class Scores final
{
public:
	enum class Type_T { normal, CE, IE };
private:
	Type_T type;
	dxle::tstring source_name;
	std::vector<Score> scores;
	dxle::tstring C_message;//�R���p�C�����b�Z�[�W
public:
	Scores() = default;
	Scores(Type_T type_, dxle::tstring source_name_, std::vector<Score> scores_, dxle::tstring C_message_)
		:type(type_), source_name(std::move(source_name_)), scores(std::move(scores_)), C_message(C_message_)
	{}
	Scores(const Scores&) = default;
	Scores(Scores&&) = default;

	Type_T get_type()const { return type; }
	const std::vector<Score>& get_scores()const { return scores; }
};

class Problem final
{
private:
	const int max_score = 0;
	const std::vector<std::pair<int, size_t>> partial_scores;//�����_�@first: ���_�A�@second�F�ǂ��܂ł̖�肩(<=)(���̓t�@�C���̔ԍ�)
	const dxle::tstring name;
	int my_socre = 0;
	std::vector<Scores> scores_set;
public:
	struct init_error{};
	//@param path:������\����/
	Problem(dxle::tstring path, const TCHAR* name)throw(...);
	Problem(const Problem&) = default;
	Problem(Problem&&) = default;

	//�X���b�h�Z�[�t
	const dxle::tstring& GetName()const { return name; }
	//�X���b�h�Z�[�t�łȂ�
	int GetScore()const { return my_socre; }
	//�X���b�h�Z�[�t
	int GetMaxScore()const { return max_score; }
	//�X���b�h�Z�[�t�łȂ�
	void AddScores(Scores&& new_data);
};
class Data final : boost::noncopyable
{
private:
	dxle::tstring user_name;

	const dxle::tstring problems_directory;//InitProblem�ȊO�ŕύX���Ȃ����Ɓi����ɂ���ăX���b�h�Z�[�t�ɂ��Ă���ׁj
	const std::vector<Problem> problems;//InitProblem�ȊO�ŗv�f����ύX���Ȃ����Ɓi����ɂ���ăX���b�h�Z�[�t�ɂ��Ă���ׁj
	std::vector<dxle::screen> problems_text;//��蕶�̃L���b�V��

	//��蕶�̔񓯊��ǂݍ���
	enum class Load_State{file_open, loading, size_checking, drawing, end}load_state;
	size_t text_total_size;//��蕶�̍��v�T�C�Y(�ʐ�)
	int viewing_problem;//�Ō�ɓǂݍ��݂��v�����ꂽ���iindex�j
	int now_loding_problem;//���ǂݍ��ݒ�(loading)�̖��iindex�j
	tifstream problem_file;//���ǂݍ��ݒ�(loading)�̖��ifile-stream�j
	dxle::pointi problem_text_next_start_pos;//��drawing�̖��̎��ɕ`����n�߂���W(X==0&&y==problem_text_newlinw_start_y�̂Ƃ��V�����s)
	int problem_text_newlinw_start_y;//��drawing�̖��̎��̍s�̊J�ny���W
	dxle::sizei problem_text_total_size;//��drawing�̖��̍ő�T�C�Y
	//�X�N���v�g
	//���ꖽ��
	//@image[width hright][image-name] //�摜��\�����܂�(width,height�� -1 or �ȗ������ꍇ�f�t�H���g�ɂȂ�܂�)
	//@h[text]//���o���̂悤�ɑ傫���A�����\�����܂�
	//@[text]//�����\�����܂�
	//@@ //@��\�����܂�
	//\n ���s���܂�
	std::vector<dxle::tstring> problem_script;//��ꖽ�߁I�I
	decltype(problem_script)::iterator problem_script_iter;//��drawing�̖��̎���͂��ׂ��X�N���v�g
	int font_normal;
	int font_boldface;//����
	int font_h1;//�啶���A����

	void InitBuildProblemText();
	void BuildProblemText();
private:
	Data();
	Data(const Data&) = delete;
	~Data();
public:
	static Data& GetIns(){
		static Data ins; return ins;
	}
	void InitProblem(dxle::tstring path);//����Ăяo������I
	void set_user_name(dxle::tstring name) { user_name = std::move(name); }
	const dxle::tstring& get_user_name() { return user_name; }
	void SetBuildProblemText(size_t index);
	void update();
	
	//@return true:finish
	bool IsLoadFinesed(int index)const {
		return problems_text[index].valid();
	}
	void DrawProblem(int index, dxle::pointi pos)const {
		problems_text[index].DrawGraph(pos, false);
	}
	void ClearProblemsCash();
	//�X���b�h�Z�[�t
	const dxle::tstring& GetProblemsDirectory()const { return problems_directory; }
	//�X���b�h�Z�[�t
	const Problem& operator[](size_t i)const{
		return problems[i];
	}
	//�X���b�h�Z�[�t
	Problem& operator[](size_t i){
		return const_cast<std::vector<Problem>&>(problems)[i];
	}
	//�X���b�h�Z�[�t
	size_t size()const { return problems.size(); }

private:
	//problems��Score�̍X�V�L���b�V��
	static std::mutex new_scores_mtx;
	static std::vector<std::pair<size_t, Scores>> new_scores;//FIFO (first: pop, last: push)
	void update_ScoresSet();
public:
	static void AddScoresSet_threadsafe(size_t problem_num, Scores new_scores);
};
