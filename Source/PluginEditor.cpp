/*
    PluginEditor.cpp
    Copyright (C) 2017 Jonathon Racz, ROLI Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioFilePlayerEditor::AudioFilePlayerEditor(AudioFilePlayerProcessor& p) :
    AudioProcessorEditor(&p),
    processor(p)
{
    thumbnail = std::make_unique<AudioThumbnailComp>(processor.formatManager, processor.transportSource, processor.thumbnailCache, processor.currentlyLoadedFile);
    addAndMakeVisible(*thumbnail);
    thumbnail->addChangeListener(this);

    formatManager.registerBasicFormats();

    addAndMakeVisible(startStopButton);
    startStopButton.setButtonText("Play/Stop");
    startStopButton.addListener(this);
    startStopButton.setColour(TextButton::buttonColourId, Colour(0xff79ed7f));

    addAndMakeVisible(openButton);
    openButton.setButtonText ("Open...");
    openButton.addListener(this);
    setOpaque(true);

    setSize(512, 252);

}

AudioFilePlayerEditor::~AudioFilePlayerEditor()
{
    thumbnail->removeChangeListener(this);
}

void AudioFilePlayerEditor::paint(Graphics& g)
{
    g.fillAll(Colours::grey);
}

void AudioFilePlayerEditor::resized()
{
    Rectangle<int> r(getLocalBounds().reduced(4));
    const Rectangle<int> openControls(r.removeFromBottom(32));
    const Rectangle<int> startStopControls(r.removeFromBottom(64));

    openButton.setBounds(openControls);
    startStopButton.setBounds(startStopControls);

    r.removeFromBottom(6);
    thumbnail->setBounds(r.removeFromBottom(180));
    r.removeFromBottom(6);
}

void AudioFilePlayerEditor::buttonClicked (Button* buttonThatWasClicked) 
{
    if (buttonThatWasClicked == &startStopButton)
    {
      if (processor.transportSource.isPlaying())
        {
          processor.transportSource.stop();
        }
      else
        {
          processor.transportSource.setPosition(0);
          processor.transportSource.start();
        }
    }
    if (buttonThatWasClicked == &openButton)
    {
      juce::FileChooser chooser ("Select a Wave or MP3 file to play...",
                                 {},
                                 "*.wav,*.mp3");

      if (chooser.browseForFileToOpen())
      {
        auto file = chooser.getResult();
        std::unique_ptr<AudioFormatReader> reader (formatManager.createReaderFor(file));

        if (reader != nullptr)
        {
          processor.loadFileIntoTransport(file);
          thumbnail->setFile(file);
        }

      }
    }
}

void AudioFilePlayerEditor::changeListenerCallback(ChangeBroadcaster* source) 
{
    if (source == thumbnail.get())
    {
        processor.loadFileIntoTransport(thumbnail->getLastDroppedFile());
        thumbnail->setFile(thumbnail->getLastDroppedFile());
    }
}
