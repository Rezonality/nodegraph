#pragma once

#include <map>
#include <memory>
#include <cmath>
#include <string>
#include <cstdint>

#include <utils/wavetable.h>

#include <signals/signals.hpp>

extern "C"
{
#include <soundpipe_extensions/soundpipeextension.h>
}

namespace NodeGraph
{
class Node;
class Canvas;
class WaveSlider;
class Slider;
}

class Oscillator
{
public:
    Oscillator(const std::string& strName, AudioUtils::WaveTableType type, float frequency = 440.0f, float phase = 0.0f);
    virtual ~Oscillator();

    void UpdateWave();
    void Reset();
    void CleanUp(); 
    /*
    virtual void Compute() override;
    */
    
    enum class WaveType
    {
        Triangle,
        Square,
        PWM,
        Saw
    };

    virtual void BuildNode(NodeGraph::Canvas& canvas);

protected:

    std::vector<fteng::connection> m_connections;

    // Inputs
    /*
    NodeGraph::Pin* m_pWave;
    NodeGraph::Pin* m_pFrequency;
    NodeGraph::Pin* m_pDetune;
    NodeGraph::Pin* m_pAmplitude;
    NodeGraph::Pin* m_pPhase;
    NodeGraph::Pin* m_pSemitones;

    NodeGraph::Pin* m_pLFO;

    NodeGraph::Pin* m_pEnablePitchMod;
    NodeGraph::Pin* m_pEnableDetuneMod;

    // Outputs
    NodeGraph::Pin* m_pOutput;
    */

    float m_phase = 0.0;
    std::map<uint32_t, sp_oscmorph2d*> m_mapOsc;

    std::vector<sp_ftbl*> m_vecTables;
    std::vector<float> m_vecTableFrequencies;
    
    int m_numBandLimitedTables = 0;

    std::shared_ptr<NodeGraph::Node> m_spNode;
    std::shared_ptr<NodeGraph::WaveSlider> m_spWaveSlider;
    std::shared_ptr<NodeGraph::Slider> m_spAmplitude;
};

