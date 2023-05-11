#include <cassert>
#include <nodes/node_oscillator.h>
#include <utils/wavetable.h>

#include <nodegraph/logger/logger.h>

#include <libs/zing/libs/earlevel/el_wavetable.h>
#include <libs/zing/libs/earlevel/el_wavetable_utils.h>

namespace
{
const int NumWaves = 4;
}

using namespace AudioUtils;

struct AudioSettings
{
    uint32_t sampleRate = 22000;
    sp_data* pSP = nullptr;
};

AudioSettings maud;

namespace Nodegraph
{

Oscillator::~Oscillator()
{
    //CleanUp();
}

void Oscillator::CleanUp()
{
    /*
    m_outFlow.FreeChannels();
    for (auto& [id, pOsc] : m_mapOsc)
    {
        sp_oscmorph2d_destroy(&pOsc);
    }
    for (auto& pTable : m_vecTables)
    {
        sp_ftbl_destroy(&pTable);
    }

    m_mapOsc.clear();
    m_vecTableFrequencies.clear();
    m_vecTables.clear();
    */
}

Oscillator::Oscillator( const std::string& strName, WaveTableType t, float f, float p)
    : m_phase(p)
{
    // Output pins
    /*
    m_pOutput = AddOutputFlow("Flow", (IFlowData*)&m_outFlow);

    m_pWave = AddInput("WaveType", 0.0f);
    ParameterAttributes waveSlider(ParameterUI::Slider, 0.0f, 1.0f);
    waveSlider.step = .05f;
    waveSlider.thumb = .25f;
    waveSlider.displayType = ParameterDisplayType::None;
    m_pWave->SetAttributes(waveSlider);

    m_pFrequency = AddInput("Frequency", 440.0f, ParameterAttributes(ParameterUI::Slider, 0.0f, 22000.0f));
    m_pFrequency->GetAttributes().flags = ParameterFlags::ReadOnly;
    m_pFrequency->GetAttributes().postFix = "Hz";
    m_pFrequency->GetAttributes().taper = 2.0f;
    m_pFrequency->GetAttributes().step = .05f;

    m_pLFO = AddInputFlow("LFO", (IFlowData*)nullptr);

    m_pAmplitude = AddInput("Amplitude", .5f, ParameterAttributes(ParameterUI::Slider, 0.0f, 1.0f));
    m_pAmplitude->GetAttributes().displayType = ParameterDisplayType::Percentage;
    m_pAmplitude->GetAttributes().step = .05f;

    m_pSemitones = AddInput("Semitones", (int64_t)0, ParameterAttributes(ParameterUI::Slider, (int64_t)-12ll, (int64_t)24));
    m_pSemitones->GetAttributes().postFix = "Semitones";

    m_pDetune = AddInput("Detune", 0.0f, ParameterAttributes(ParameterUI::Slider, -4.0f, 4.0f));
    m_pDetune->GetAttributes().step = .05f;

    m_pPhase = AddInput("Phase", 0.0f, ParameterAttributes(ParameterUI::Slider, 0.0f, 1.0f));
    m_pPhase->GetAttributes().step = .05f;

    m_pEnablePitchMod = AddInputRadio("PitchLFO", { "Pitch 0", "Pitch 1" }, true);

    m_pEnableDetuneMod = AddInputRadio("DetuneLFO", { "Detune 0", "Detune 1" }, true);

    auto pLineDecorator = AddDecorator(new NodeDecorator(DecoratorType::Line));
    auto pDecorator = AddDecorator(new NodeDecorator(DecoratorType::Label, "LFO Modulators"));

    {
        /*NVec2f knobSize(1.0f, 1.0f);
        //GridLayout layout(this);
        //layout.BeginSection();
        layout.AddItem(m_pWave, NVec2f(2.0f, .5f));

        //layout.BeginSection();

        layout.AddItem(m_pFrequency, knobSize);
        layout.AddItem(m_pDetune, knobSize);

        //layout.BeginSection();
        layout.AddItem(m_pSemitones, knobSize);
        layout.AddItem(m_pAmplitude, knobSize);

        //layout.BeginSection(true);
        layout.AddDecorator(pLineDecorator, NVec2f(knobSize.x, .125f));

        //layout.BeginSection(true);
        layout.AddDecorator(pDecorator, NVec2f(ButtonGridWidthForString("LFO Modulators"), .333f));

        //layout.BeginSection(true);
        layout.AddItem(m_pEnablePitchMod, NVec2f(buttonSize, .5f));

        //layout.BeginSection(true);
        layout.AddItem(m_pEnableDetuneMod, NVec2f(buttonSize, .5f));
        */

    /*
        auto sliderSize = NVec2f(200.0f, 30.0f);
        auto buttonSize = NVec2f(200.0f, 50.0f);

        auto pHLayout = new HLayout();
        auto pVLayout1 = new VLayout();
        auto pVLayout2 = new VLayout();
        pHLayout->AddItem(pVLayout1);
        pHLayout->AddItem(pVLayout2);
        GetLayout().spContents->AddItem(pHLayout);

        pVLayout1->AddItem(m_pWave, sliderSize);
        pVLayout1->AddItem(m_pFrequency, sliderSize);
        pVLayout1->AddItem(m_pDetune, sliderSize);
        pVLayout1->AddItem(m_pSemitones, sliderSize);

        pVLayout2->AddItem(m_pAmplitude, sliderSize);
        pVLayout2->AddItem(m_pEnablePitchMod, buttonSize);
        pVLayout2->AddItem(m_pEnableDetuneMod, buttonSize);
    }

    Reset();
    */
}

void Oscillator::Reset()
{
    CleanUp();

    int tableLen = 2048;

    // The wave table to use
    std::vector<WaveTable> waves;
    waves.resize(NumWaves);
    wave_table_create(waves[0], WaveTableType::Triangle, m_phase, tableLen);
    wave_table_create(waves[1], WaveTableType::Square, m_phase, tableLen);
    wave_table_create(waves[2], WaveTableType::SquarePWM, m_phase, tableLen);
    wave_table_create(waves[3], WaveTableType::ReverseSawtooth, m_phase, tableLen);

    std::vector<Earlevel::WaveTableOsc*> waveTables;
    m_numBandLimitedTables = 0;

    for (auto& wave : waves)
    {
        int idx;
        double* freqWaveRe = new double[tableLen];
        double* freqWaveIm = new double[tableLen];

        // take FFT
        for (idx = 0; idx < tableLen; idx++)
        {
            freqWaveIm[idx] = float(wave.data[idx]);
            freqWaveRe[idx] = 0.0;
        }
        Earlevel::fft(tableLen, freqWaveRe, freqWaveIm);

        // build a wavetable oscillator
        Earlevel::WaveTableOsc* osc = new Earlevel::WaveTableOsc();
        Earlevel::fillTables(osc, freqWaveRe, freqWaveIm, tableLen);

        waveTables.push_back(osc);

        if (m_vecTableFrequencies.empty())
        {
            m_numBandLimitedTables = osc->GetNumTables();
            m_vecTableFrequencies.resize(m_numBandLimitedTables);
            for (int table = 0; table < m_numBandLimitedTables; table++)
            {
                m_vecTableFrequencies[table] = float(osc->GetTables()[table].topFreq * maud.sampleRate);
            }
        }
        // Ensure all have the same table size
        assert(m_numBandLimitedTables == osc->GetNumTables());

        delete[] freqWaveRe;
        delete[] freqWaveIm;
    }

    m_vecTables.clear();
    for (int subTable = 0; subTable < m_numBandLimitedTables; subTable++)
    {
        for (int wave = 0; wave < NumWaves; wave++)
        {
            auto& sourceTable = waveTables[wave]->GetTables()[subTable];

            // Create the wave table and copy in the data
            sp_ftbl* pTable = nullptr;
            sp_ftbl_create(maud.pSP, &pTable, sourceTable.waveTableLen);

            // Create our wavetable for the oscillator
            for (size_t i = 0; i < sourceTable.waveTableLen; i++)
            {
                pTable->tbl[i] = sourceTable.waveTable[i];
            }

            m_vecTables.push_back(pTable);
        }
    }

    for (auto& osc : waveTables)
    {
        delete osc;
    }
}

/*
void Oscillator::Compute()
{
    PROFILE_SCOPE(Osc);

    // Not sure what this does
    double currentSampleIncrement = 1.0 * SP_FT_MAXLEN / maud.pSP->sr;
    for (auto& table : m_vecTables)
    {
        table->sicvt = (float)currentSampleIncrement;
    }

    const auto& modulators = GetModulators(*m_pLFO);

    // Notes at higher frequencies are further apart, so need detune to get progressively larger
    auto semitones = m_pSemitones->GetValue<int64_t>();
    auto amp = m_pAmplitude->GetValue<float>();
    auto phase = m_pPhase->GetValue<float>();

    uint32_t channelIndex = 0;

    auto AddChannel = [&](uint32_t id, float freq) {
        sp_oscmorph2d* pOsc = nullptr;
        auto itrOsc = m_mapOsc.find(id);
        if (itrOsc == m_mapOsc.end())
        {
            sp_oscmorph2d_create(&pOsc);

            // Setup the oscillator
            sp_oscmorph2d_init(maud.pSP, pOsc, &m_vecTables[0], NumWaves, m_numBandLimitedTables, &m_vecTableFrequencies[0], 0);
            pOsc->freq = 0;
            pOsc->amp = 0;
            pOsc->wtpos = 0;
            pOsc->enableBandlimit = 1;
            pOsc->bandlimitIndexOverride = -1;

            m_mapOsc.insert(std::make_pair(id, pOsc));
        }
        else
        {
            pOsc = itrOsc->second;
        }
        pOsc->wtpos = m_pWave->To<float>();

        float noteFrequency = freq;
        float noteAmplitude = amp;

        //m_pFrequency->SetFrom<float>(noteFrequency);

        pOsc->amp = noteAmplitude;
        pOsc->iphs = phase;

        auto detuneOption = m_pEnableDetuneMod->To<int64_t>();
        auto pitchOption = m_pEnablePitchMod->To<int64_t>();

        auto semitoneFreq = noteFrequency * maud_interval_to_frequency_ratio((float)semitones);

        auto pOut = m_outFlow.GetChannelById(id, maud.genFrameCount)->Ptr<float>();
        float lfo[2];
        for (uint32_t i = 0; i < maud.genFrameCount; i++)
        {
            auto freq = semitoneFreq;

            auto lfoAdd = UpdateModulatorValues(lfo, 2, i);

            //pitchLFO common frequency coefficient
            float pitchLFOCoefficient = 1.f;
            const float semitone = 0.0594630944f; // 1 = 2^(1/12)
            if (pitchOption == 1)
            {
                pitchLFOCoefficient = 1.f + lfo[0] * semitone;
            }
            if (pitchOption == 2)
            {
                pitchLFOCoefficient = 1.f + lfo[1] * semitone;
            }
            else if (pitchOption == 3)
            {
                pitchLFOCoefficient = 1.f + lfoAdd * semitone;
            }

            // Pitch LFO
            freq *= pitchLFOCoefficient;

            // Detune LFO
            // relative to the center of the note
            const float relativeDetune = noteFrequency / 261.6255653006f;
            auto detune = m_pDetune->GetValue<float>() * relativeDetune;
            if (detuneOption == 1)
            {
                freq += lfo[0] * detune;
            }
            else if (detuneOption == 2)
            {
                freq += lfo[1] * detune;
            }
            else if (detuneOption == 3)
            {
                freq += lfoAdd * detune;
            }
            else
            {
                freq += detune;
            }

            freq = std::clamp(freq, 0.0f, .5f * maud.sampleRate);

            pOsc->freq = freq;

            sp_oscmorph2d_compute(maud.pSP, pOsc, nullptr, &pOut[i]);
        }
    };

    bool hasOutputs = (!m_pOutput->GetTargets().empty());
    m_pFrequency->GetAttributes().flags |= ParameterFlags::ReadOnly;

    auto& notes = GetAudioGraph().GetActiveNotes();
    
    m_outFlow.FreeChannels();

    for (auto& [id, note] : notes)
    {
        AddChannel(id, note->frequency);

        // For the simple unconnected oscillator case, handle notes that go nowhere
        if (!hasOutputs)
        {
            // Inform the instrument that this note is not inactive
            // Since this oscillator will run while a note arrives
            note->NotifyActive(note->Pressed());
        }
    }
}

*/

} // namespace MAudio
