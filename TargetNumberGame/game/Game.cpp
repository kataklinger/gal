
#include "Game.h"
#include "..\parsing\Parsing.h"
#include "..\..\GeneticLibrary\source\Population.h"
#include "..\..\GeneticLibrary\source\SelectionOperations.h"
#include "..\..\GeneticLibrary\source\CouplingOperations.h"
#include "..\..\GeneticLibrary\source\ReplacementOperations.h"
#include "..\..\GeneticLibrary\source\StopCriterias.h"

namespace Game
{

	inline void MoveControlFocus(HWND control) { SendMessage( ::GetParent( control ), WM_NEXTDLGCTL, (WPARAM)control, TRUE ); }

	void NumberGenerator::Generate(bool user)
	{
		Clear();

		if( user )
		{
			_currentNumber = -1;
			NextUser();
			UserGenerateLoop();
		}
		else
		{
			for( int i = 0; i < NUMBERS_TO_GENERATE; i++ )
			{
				if( i < 4 )
					_generatedNumbers[ i ] = GetRandomNumber( 1, 9 );
				else if( i == 4 )
					_generatedNumbers[ i ] = 10 + GetRandomNumber( 0, 2 ) * 5;
				else if( i == 5 )
					_generatedNumbers[ i ] = 25 + GetRandomNumber( 0, 3 ) * 25;
				else
					_generatedNumbers[ i ] = GetRandomNumber( 101, 999 );

				_numberButtons[ i ]->ShowWindow( SW_SHOW );
				_numberButtons[ i ]->EnableWindow( FALSE );

				SetButtonText( _numberButtons[ i ], _generatedNumbers[ i ] );
			}
		}
	}

	bool NumberGenerator::NextUser()
	{
		if( _currentNumber >= 0 )
			_numberButtons[ _currentNumber ]->EnableWindow( FALSE );

		if( _currentNumber + 1 < NUMBERS_TO_GENERATE )
		{
			_numberButtons[ _currentNumber + 1 ]->EnableWindow( TRUE );
			_numberButtons[ _currentNumber + 1 ]->ShowWindow( SW_SHOW );

			MoveControlFocus( *_numberButtons[ _currentNumber + 1 ] );
		}

		_currentNumber++;

		return _currentNumber == NUMBERS_TO_GENERATE;
	}

	void NumberGenerator::Clear()
	{
		for( int i = 0; i < NUMBERS_TO_GENERATE; i++ )
		{
			_numberButtons[ i ]->SetWindowText( _T( "" ) );
			_numberButtons[ i ]->EnableWindow( TRUE );
			_numberButtons[ i ]->ShowWindow( SW_HIDE );

			_generatedNumbers[ i ] = -1;
		}
	}

	void NumberGenerator::SetButtonText(CButton* button,
		int number)
	{
		CString str;
		str.Format( _T( "%d" ), number );

		button->SetWindowText( str );
	}

	void NumberGenerator::UserGenerateLoop()
	{
		while( 1 )
		{
			int index = _currentNumber;

			CButton* currentButton = _numberButtons[ index ];
			if( !currentButton )
				break;

			int* currentNumber = _generatedNumbers + index;

			if( index < 4 )
				*currentNumber = GetRandomNumber( 1, 9 );
			else if( index == 4 )
				*currentNumber = 10 + GetRandomNumber( 0, 2 ) * 5;
			else if( index == 5 )
				*currentNumber = 25 + GetRandomNumber( 0, 3 ) * 25;
			else
				*currentNumber = GetRandomNumber( 101, 999 );

			SetButtonText( currentButton, *currentNumber );

			::Sleep( 50 );
		}
	}

	void Timer::Start()
	{
		_timeLeft = _gameLength;
		_timeControl->SetRange( 0, _timeLeft );
		_timeControl->SetStep( 1 );
		_timeControl->SetPos( _timeLeft );

		_active = true;
		TimerLoop();
	}

	void Timer::TimerLoop()
	{
		while( _timeLeft > 0 && _active )
		{
			::Sleep( 1000 );
			_timeControl->SetPos( --_timeLeft );
		}
	}

	void InputManager::EnableInput(bool enable)
	{
		_formula->EnableWindow( enable );
		_result->EnableWindow( enable );
		_doneBtn->EnableWindow( enable );

		if( enable )
		{
			MoveControlFocus( *_formula );

			_formula->SetWindowText( _T( "" ) );
			_result->SetWindowText( _T( "" ) );
			_syntax->SetWindowText( _T( "Input: Correct!" ) );
		}
	}

	void InputManager::CheckInput(const int* numbers,
		int count)
	{
		CString input;
		_formula->GetWindowText( input );

		Parsing::Parser parser( std::string( input.GetString(), input.GetString() + input.GetLength() ) );

		try
		{
			parser.Parse( count, numbers );
			_syntax->SetWindowText( _T( "Input: Correct!" ) );
		}
		catch(Parsing::SyntaxException& ex)
		{
			CString error;
			error.Format( _T( "Input: Syntax error at (%d)!" ), ex._charPosition );
			_syntax->SetWindowText( error );
		}
		catch(Parsing::ArithmeticException&)
		{
			_syntax->SetWindowText(  _T( "Input: Division by zero or divisiond does not yield integer!" ) );
		}
		catch(Parsing::InputException& ex)
		{
			CString error;
			error.Format( _T( "Input: Illegal number used (%d)!" ), ex._value );
			_syntax->SetWindowText( error );
		}
	}

	void InputManager::GrabInput(Results* results,
		const int* numbers,
		int count)
	{
		EnableInput( false );

		CString input, result;
		_formula->GetWindowText( input );
		_result->GetWindowText( result );

		Parsing::Parser parser( std::string( input.GetString(), input.GetString() + input.GetLength() ) );

		try
		{
			results->_userResult = parser.Parse( count, numbers );
			results->_userReported = _wtoi( result.GetString() );
			results->_inputCorrect = true;
		}
		catch( ... )
		{
			results->_userResult = 0;
			results->_userReported = 0;
			results->_inputCorrect = false;
		}
	}

	void CpuPlayer::StatisticUpdate(const Common::GaStatistics& statistics,
		const Algorithm::GaAlgorithm& algorithm)
	{
		if( statistics.GetCurrentGeneration() % 1000 == 0 )
		{
			CString str;
			str.Format( _T( "CPU status: %d generation(s)" ), statistics.GetCurrentGeneration() );
			_cpuStatus->SetWindowText( str );
		}
	}

	void CpuPlayer::NewBestChromosome(const Chromosome::GaChromosome& newChromosome,
		const Algorithm::GaAlgorithm& algorithm)
	{
		OutputPopulation( algorithm.GetPopulation( 0 ) );
	}

	void CpuPlayer::EvolutionStateChanged(Algorithm::GaAlgorithmState newState,
		const Algorithm::GaAlgorithm& algorithm)
	{
		if( newState == Algorithm::GAS_RUNNING )
			_cpuStatus->SetWindowText( _T( "CPU status: Solving!" ) );
		else if( newState & Algorithm::GAS_NOT_RUNNING )
		{
			_algorithm = NULL;

			const int* numbers = ( (TNG::TngConfigBlock&)( (TNG::TngChromosome&)algorithm.GetPopulation( 0 ).GetPrototype() ).GetConfigBlock() ).GetNumbers();
			_result = TNG::TngCalculateValue( ( (TNG::TngChromosome&)*algorithm.GetPopulation( 0 ).GetAt( 0 ).GetChromosome() ).GetRoot(), numbers );
			_formula = TNG::TngToString( (TNG::TngChromosome&)*algorithm.GetPopulation( 0 ).GetAt( 0 ).GetChromosome() ).c_str();

			OutputPopulation( algorithm.GetPopulation( 0 ) );

			_cpuStatus->SetWindowText( _T( "CPU status: Ready!" ) );
		}
	}

	void CpuPlayer::OutputPopulation(const Population::GaPopulation& population)
	{
		for( int i =population.GetCurrentSize() - 1; i >= 0; i-- )
		{
			std::string str = TNG::TngToString( (TNG::TngChromosome&)*population.GetAt( i ).GetChromosome() );
			_populationContent->InsertItem( 0, CString( str.c_str() ) );

			CString fitness;
			fitness.Format( _T( "%5.2f" ), population.GetAt( i ).GetFitnessForComparison() );
			_populationContent->SetItemText( 0, 1, fitness );
		}

		CString generation;
		generation.Format( _T( "---- Generation %d ----" ), population.GetStatistics().GetCurrentGeneration() );
		_populationContent->InsertItem( 0, generation );

	}

	CpuPlayer::CpuPlayer(CStatic* cpuStatus,
		CListCtrl* populationContent) : _result(0), _cpuStatus(cpuStatus), _populationContent(populationContent), _algorithm(NULL),
		_chromosomeParams( /*0.3f*/0.3f, 1, true, 0.8f, 1 ), _configBlock( &_crossover, &_mutation,
			&_fitnessOperation, GaFitnessComparatorCatalogue::Instance().GetEntryData( "GaMaxFitnessComparator" ), &_chromosomeParams ),
		_populationParams( 32, false, true, false, 0, 0 ),
		_populationConfig( _populationParams, &_configBlock.GetFitnessComparator(),
			GaSelectionCatalogue::Instance().GetEntryData( "GaSelectRouletteWheel" ), &Population::SelectionOperations::GaSelectDuplicatesParams( false, 8 ),
			GaReplacementCatalogue::Instance().GetEntryData( "GaReplaceWorst" ), &Population::GaReplacementParams( 8 ),
			GaCouplingCatalogue::Instance().GetEntryData( "GaSimpleCoupling" ), &Population::GaCouplingParams( 8, true ),
			NULL, NULL),
		_algorithmParams( 2 ) { }

	CpuPlayer::~CpuPlayer() { }

	void CpuPlayer::Start(const NumberGenerator& numbers)
	{
		_populationContent->DeleteAllItems();

		_configBlock.SetNumbers( numbers.GetGenerated() );
		_configBlock.SetTargetNumber( numbers[ NumberGenerator::NUMBERS_TO_GENERATE - 1 ] );

		TNG::TngChromosome prototype( &_configBlock );
		Population::GaPopulation population( &prototype, &_populationConfig );
		Algorithm::SimpleAlgorithms::GaIncrementalAlgorithm algorithm( &population, _algorithmParams );

		_algorithm = &algorithm; 

		TNG::TngStopCriteriaParams criteriaParams( GAME_TIME - 2 );
		algorithm.SetStopCriteria( &_stopCriteria, &criteriaParams);

		algorithm.SubscribeObserver( this );

		algorithm.StartSolving( false );
		algorithm.WaitForThreads();
	}

	void CpuPlayer::Stop()
	{
		if( _algorithm )
			_algorithm->StopSolving();
	}

	void Master::StartGame()
	{
		_userReady = _cpuReady = false;
		_input.EnableInput( false );

		::CloseHandle( ::CreateThread( NULL, 0, GameFlowWrapper, this, 0, NULL ) );
	}

	void Master::ReportReady(bool user)
	{
		user ? _userReady = true : _cpuReady = true;

		if( _userReady )
		{
			_input.EnableInput( false );

			if( _cpuReady )
				_timer.Stop();
		}
	}

	void Master::GameFlow()
	{
		_generator.Generate( _userTurn );
		_results._target = GetNumberGenerator()[ NumberGenerator::NUMBERS_TO_GENERATE - 1 ];

		::CloseHandle( ::CreateThread( NULL, 0, CpuPlayer, this, 0, NULL ) );
		_input.EnableInput( true );
		_timer.Start();

		_input.GrabInput( &_results, _generator.GetGenerated(), NumberGenerator::NUMBER_COUNT );

		_results._cpuResult = _cpuPlayer.GetResult();
		_results._cpuFormula = _cpuPlayer.GetFormula();
		_results._userTurn = _userTurn;

		_userTurn = !_userTurn;

		_window->SendMessage( WM_GAME_FINISHED, 0, 0 );
	}

	DWORD Master::CpuPlayer(LPVOID param)
	{
		Master* pThis = (Master*)param;
		pThis->_cpuPlayer.Start( pThis->GetNumberGenerator() );
		pThis->ReportReady( false );

		return 0;
	}

} // Game
