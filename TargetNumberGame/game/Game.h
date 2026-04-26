
#ifndef __TNG_GAME_H__
#define __TNG_GAME_H__

#include <afxwin.h>
#include <afxcmn.h>

#include "..\..\GeneticLibrary\source\GlobalRandomGenerator.h"
#include "..\..\GeneticLibrary\source\IncrementalAlgorithm.h"

#include "..\algorithm\TNG.h"

const int GAME_TIME = 60;

#define WM_GAME_FINISHED (WM_USER + 1)

namespace Game
{

	struct Results
	{
		bool _userTurn;

		bool _inputCorrect;
		CString _userFormula;
		int _userResult;
		int _userReported;

		CString _cpuFormula;
		int _cpuResult;

		int _target;
	};

	class NumberGenerator
	{

	public:

		static const int NUMBER_COUNT = 6; 

		static const int NUMBERS_TO_GENERATE = NUMBER_COUNT + 1;

	private:

		CButton* _numberButtons[ NUMBERS_TO_GENERATE + 1 ];

		int _generatedNumbers[ NUMBERS_TO_GENERATE ];

		volatile int _currentNumber;

	public:

		NumberGenerator() : _currentNumber(0) { _numberButtons[ NUMBERS_TO_GENERATE ] = NULL; }

		void BindButton(CButton* button,
			int boundNumber) { _numberButtons[ boundNumber ] = button; }

		void Generate(bool user);

		bool NextUser();

		void Clear();

		inline const int* GetGenerated() const { return _generatedNumbers; }

		inline int operator [](int index) const { return _generatedNumbers[ index ]; }

	private:

		inline static int GetRandomNumber(int min, 
			int max) { return GaGlobalRandomIntegerGenerator->Generate( min, max ); }

		static void SetButtonText(CButton* button,
			int number);

		void UserGenerateLoop();

	};

	class Timer
	{

	private:

		CProgressCtrl* _timeControl;

		int _gameLength;

		int _timeLeft;

		volatile bool _active;

	public:

		Timer(int gameLength,
			CProgressCtrl* timeControl) : _gameLength(gameLength), _timeControl(timeControl), _timeLeft(0), _active(false) {  }

		void Start();

		inline void Stop() { _active = false; }

	private:

		void TimerLoop();

	};

	class InputManager
	{

	private:

		CEdit* _formula;

		CEdit* _result;

		CButton* _doneBtn;

		CStatic* _syntax;

	public:

		InputManager(CEdit* formula,
			CEdit* result,
			CButton* doneBtn,
			CStatic* syntax) : _formula(formula),
			_result(result),
			_doneBtn(doneBtn),
			_syntax(syntax) { }

		void EnableInput(bool enable);

		void CheckInput(const int* numbers,
			int count);

		void GrabInput(Results* results,
			const int* numbers,
			int count);

	};

	class CpuPlayer : public Observing::GaObserver
	{

	private:

		CString _formula;

		int _result;

		CStatic* _cpuStatus;

		CListCtrl* _populationContent;

		TNG::TngCrossover _crossover;

		TNG::TngMutation _mutation;

		TNG::TngFitnessOperation _fitnessOperation;

		TNG::TngStopCriteria _stopCriteria;

		Chromosome::GaChromosomeParams _chromosomeParams;

		TNG::TngConfigBlock _configBlock;

		Population::GaPopulationParameters _populationParams;

		Population::GaPopulationConfiguration _populationConfig;

		Algorithm::GaMultithreadingAlgorithmParams _algorithmParams;

		Algorithm::GaMultithreadingAlgorithm* _algorithm;

	public:

		CpuPlayer(CStatic* cpuStatus,
			CListCtrl* populationContent);

		~CpuPlayer();

		void Start(const NumberGenerator& numbers);

		void Stop();

		inline const CString& GetFormula() const { return _formula; }

		inline int GetResult() const { return _result; }

		virtual void GACALL StatisticUpdate(const Common::GaStatistics& statistics,
			const Algorithm::GaAlgorithm& algorithm);

		virtual void GACALL NewBestChromosome(const Chromosome::GaChromosome& newChromosome,
			const Algorithm::GaAlgorithm& algorithm);

		virtual void GACALL EvolutionStateChanged(Algorithm::GaAlgorithmState newState,
			const Algorithm::GaAlgorithm& algorithm);

	protected:

		void OutputPopulation(const Population::GaPopulation& population);

	};

	class Master
	{

	private:

		bool _userTurn;

		bool _userReady;

		bool _cpuReady;

		NumberGenerator _generator;

		Timer _timer;

		InputManager _input;

		CpuPlayer _cpuPlayer;

		Results _results;

		CWnd* _window;

	public:

		Master(int gameLength,
			CProgressCtrl* timeControl,
			CEdit* formula,
			CEdit* result,
			CButton* doneBtn,
			CStatic* syntax,
			CStatic* cpuStatus,
			CListCtrl* populationContent,
			CWnd* window) : _timer(gameLength, timeControl), _input(formula, result, doneBtn, syntax),
			_cpuPlayer(cpuStatus, populationContent), _userTurn(false), _userReady(false), _cpuReady(false), _window(window) { }

		void StartGame();

		void StopGame() { _cpuPlayer.Stop(); _timer.Stop(); }

		void ReportReady(bool user);

		inline NumberGenerator& GetNumberGenerator() { return _generator; }

		inline InputManager& GetInputManager() { return _input; }

		inline const Results& GetGameResult() const { return _results; }

	private:

		void GameFlow();

		static DWORD WINAPI GameFlowWrapper(LPVOID param)
		{
			( (Master*)param )->GameFlow();
			return 0;
		}

		static DWORD WINAPI CpuPlayer(LPVOID param);

	};

} // Game

#endif // __TNG_GAME_H__
