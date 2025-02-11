// Copyright (c) 2017-2019 VMware, Inc. All Rights Reserved.
// SPDX-License-Identifier: GPL-2.0

#pragma once
#include "Allocations/Describer.h"
#include "Allocations/PatternRecognizerRegistry.h"
#include "Allocations/Subcommands/DefaultSubcommands.h"
#include "Allocations/Subcommands/SummarizeSignatures.h"
#include "Commands/CountCommand.h"
#include "Commands/DescribeCommand.h"
#include "Commands/EnumerateCommand.h"
#include "Commands/ExplainCommand.h"
#include "Commands/ListCommand.h"
#include "Commands/Runner.h"
#include "Commands/ShowCommand.h"
#include "Commands/SummarizeCommand.h"
#include "CompoundDescriber.h"
#include "DequeMapRecognizer.h"
#include "DoublyLinkedListNodeRecognizer.h"
#include "InModuleDescriber.h"
#include "KnownAddressDescriber.h"
#include "MapOrSetNodeRecognizer.h"
#include "ModuleAlignmentGapDescriber.h"
#include "ModuleCommands/ListModules.h"
#include "ProcessImage.h"
#include "StackDescriber.h"
#include "StackOverflowGuardDescriber.h"
#include "ThreadMapCommands/CountStacks.h"
#include "ThreadMapCommands/DescribeStacks.h"
#include "ThreadMapCommands/ListStacks.h"
#include "UnorderedMapOrSetBucketsRecognizer.h"
#include "UnorderedMapOrSetNodeRecognizer.h"
#include "VectorBodyRecognizer.h"
#include "VirtualAddressMapCommands/CountRanges.h"
#include "VirtualAddressMapCommands/DescribeRanges.h"
#include "VirtualAddressMapCommands/ListRanges.h"
#include "VirtualAddressMapCommands/SummarizeRanges.h"

namespace chap {
template <typename Offset>
class ProcessImageCommandHandler {
 public:
  typedef ProcessImageCommandHandler<Offset> ThisClass;
  ProcessImageCommandHandler(const ProcessImage<Offset>& processImage)
      : _virtualMemoryPartition(processImage.GetVirtualMemoryPartition()),
        _stackDescriber(processImage),
        _knownAddressDescriber(processImage),
        _inModuleDescriber(processImage, _knownAddressDescriber),
        _moduleAlignmentGapDescriber(processImage),
        _stackOverflowGuardDescriber(processImage),
        _allocationDescriber(_inModuleDescriber, _stackDescriber,
                             _patternRecognizerRegistry, processImage),
        _describeCommand(_compoundDescriber),
        _explainCommand(_compoundDescriber),
        _countStacksSubcommand(processImage),
        _listStacksSubcommand(processImage),
        _describeStacksSubcommand(processImage),
        _listModulesSubcommand(processImage),
        _countInaccessibleSubcommand(
            "inaccessible",
            "This command provides totals of the number of "
            "inaccessible ranges\n(not readable, writable or "
            "executable) and the space they occupy.\n",
            "inaccessible ranges",
            _virtualMemoryPartition.GetClaimedInaccessibleRanges()),
        _summarizeInaccessibleSubcommand(
            "inaccessible",
            "This command summarizes (by use) the number of ranges and "
            "byte counts for\ninaccessible ranges (not readable, writable or "
            "executable).\n",
            "inaccessible ranges",
            _virtualMemoryPartition.GetClaimedInaccessibleRanges()),
        _listInaccessibleSubcommand(
            "inaccessible",
            "This command lists the address, limit and size of "
            "inaccessible ranges (not\nreadable, writable or "
            "executable) and gives totals for ranges and space used.\n",
            "inaccessible ranges",
            _virtualMemoryPartition.GetClaimedInaccessibleRanges()),
        _describeInaccessibleSubcommand(
            "inaccessible",
            "This command gives the address, limit, size and rough use of "
            "inaccessible ranges\n(not readable, writable or "
            "executable) and gives totals for ranges and space used.\n",
            "inaccessible ranges",
            _virtualMemoryPartition.GetClaimedInaccessibleRanges(),
            _compoundDescriber, _virtualMemoryPartition.UNKNOWN),
        _countReadOnlySubcommand(
            "readonly",
            "This command provides totals of the number of "
            "read-only ranges\nand the space they occupy.\n",
            "read-only ranges",
            _virtualMemoryPartition.GetClaimedReadOnlyRanges()),
        _summarizeReadOnlySubcommand(
            "readonly",
            "This command summarizes (by use) the number of ranges and "
            "byte counts for\nread-only ranges.\n",
            "read-only ranges",
            _virtualMemoryPartition.GetClaimedReadOnlyRanges()),
        _listReadOnlySubcommand(
            "readonly",
            "This command lists the address, limit and size of "
            "read-only ranges\nand gives totals for ranges and space used.\n",
            "read-only ranges",
            _virtualMemoryPartition.GetClaimedReadOnlyRanges()),
        _describeReadOnlySubcommand(
            "readonly",
            "This command gives the address, limit, size and rough use of "
            "read-only ranges\nand gives totals for ranges and space used.\n",
            "read-only ranges",
            _virtualMemoryPartition.GetClaimedReadOnlyRanges(),
            _compoundDescriber, _virtualMemoryPartition.UNKNOWN),
        _countRXOnlySubcommand(
            "rxonly",
            "This command provides totals of the number of "
            "rx-only ranges\nand the space they occupy.\n",
            "rx-only ranges", _virtualMemoryPartition.GetClaimedRXOnlyRanges()),
        _summarizeRXOnlySubcommand(
            "rxonly",
            "This command summarizes (by use) the number of ranges and "
            "byte counts for rx-only\nranges (readable and executable "
            "but not writable).\n",
            "rx-only ranges", _virtualMemoryPartition.GetClaimedRXOnlyRanges()),
        _listRXOnlySubcommand(
            "rxonly",
            "This command lists the address, limit and size of "
            "rx-only ranges\nand gives totals for ranges and space used.\n",
            "rx-only ranges", _virtualMemoryPartition.GetClaimedRXOnlyRanges()),
        _describeRXOnlySubcommand(
            "rxonly",
            "This command gives the address, limit, size and rough use of "
            "rx-only ranges\nand gives totals for ranges and space used.\n",
            "rx-only ranges", _virtualMemoryPartition.GetClaimedRXOnlyRanges(),
            _compoundDescriber, _virtualMemoryPartition.UNKNOWN),
        _countWritableSubcommand(
            "writable",
            "This command provides totals of the number of "
            "writable ranges\nand the space they occupy.\n",
            "writable ranges",
            _virtualMemoryPartition.GetClaimedWritableRanges()),
        _summarizeWritableSubcommand(
            "writable",
            "This command summarizes (by use) the number of ranges and "
            "byte counts for\nwritable ranges.\n",
            "writable ranges",
            _virtualMemoryPartition.GetClaimedWritableRanges()),
        _listWritableSubcommand(
            "writable",
            "This command lists the address, limit and size of "
            "writable ranges\nand gives totals for ranges and space used.\n",
            "writable ranges",
            _virtualMemoryPartition.GetClaimedWritableRanges()),
        _describeWritableSubcommand(
            "writable",
            "This command gives the address, limit, size and rough use of "
            "writable ranges\nand gives totals for ranges and space used.\n",
            "writable ranges",
            _virtualMemoryPartition.GetClaimedWritableRanges(),
            _compoundDescriber, _virtualMemoryPartition.UNKNOWN),
        _summarizeSignaturesSubcommand(processImage),
        _defaultAllocationsSubcommands(processImage, _allocationDescriber,
                                       _patternRecognizerRegistry),
        _dequeMapRecognizer(processImage),
        _unorderedMapOrSetBucketsRecognizer(processImage),
        _unorderedMapOrSetNodeRecognizer(processImage),
        _mapOrSetNodeRecognizer(processImage),
        _vectorBodyRecognizer(processImage),
        _doublyLinkedListNodeRecognizer(processImage) {
    _patternRecognizerRegistry.Register(_dequeMapRecognizer);
    _patternRecognizerRegistry.Register(_unorderedMapOrSetBucketsRecognizer);
    _patternRecognizerRegistry.Register(_unorderedMapOrSetNodeRecognizer);
    _patternRecognizerRegistry.Register(_mapOrSetNodeRecognizer);
    _patternRecognizerRegistry.Register(_vectorBodyRecognizer);
    _patternRecognizerRegistry.Register(_doublyLinkedListNodeRecognizer);
    // Leave it to any derived class to add any describers.
  }

  virtual void AddCommandCallbacks(Commands::Runner& /* r */) {}

  virtual void AddCommands(Commands::Runner& r) {
    r.AddCommand(_countCommand);
    r.AddCommand(_summarizeCommand);
    r.AddCommand(_enumerateCommand);
    r.AddCommand(_listCommand);
    r.AddCommand(_showCommand);
    r.AddCommand(_describeCommand);
    r.AddCommand(_explainCommand);
    RegisterSubcommand(r, _countStacksSubcommand);
    RegisterSubcommand(r, _listStacksSubcommand);
    RegisterSubcommand(r, _describeStacksSubcommand);
    RegisterSubcommand(r, _listModulesSubcommand);
    RegisterSubcommand(r, _countInaccessibleSubcommand);
    RegisterSubcommand(r, _summarizeInaccessibleSubcommand);
    RegisterSubcommand(r, _listInaccessibleSubcommand);
    RegisterSubcommand(r, _describeInaccessibleSubcommand);
    RegisterSubcommand(r, _countReadOnlySubcommand);
    RegisterSubcommand(r, _summarizeReadOnlySubcommand);
    RegisterSubcommand(r, _listReadOnlySubcommand);
    RegisterSubcommand(r, _describeReadOnlySubcommand);
    RegisterSubcommand(r, _countRXOnlySubcommand);
    RegisterSubcommand(r, _summarizeRXOnlySubcommand);
    RegisterSubcommand(r, _listRXOnlySubcommand);
    RegisterSubcommand(r, _describeRXOnlySubcommand);
    RegisterSubcommand(r, _countWritableSubcommand);
    RegisterSubcommand(r, _summarizeWritableSubcommand);
    RegisterSubcommand(r, _listWritableSubcommand);
    RegisterSubcommand(r, _describeWritableSubcommand);
    RegisterSubcommand(r, _summarizeSignaturesSubcommand);
    _defaultAllocationsSubcommands.RegisterSubcommands(r);
  }

 protected:
  VirtualMemoryPartition<Offset> _virtualMemoryPartition;
  StackDescriber<Offset> _stackDescriber;
  Allocations::PatternRecognizerRegistry<Offset> _patternRecognizerRegistry;
  KnownAddressDescriber<Offset> _knownAddressDescriber;
  InModuleDescriber<Offset> _inModuleDescriber;
  ModuleAlignmentGapDescriber<Offset> _moduleAlignmentGapDescriber;
  StackOverflowGuardDescriber<Offset> _stackOverflowGuardDescriber;
  Allocations::Describer<Offset> _allocationDescriber;
  CompoundDescriber<Offset> _compoundDescriber;
  Commands::CountCommand _countCommand;
  Commands::SummarizeCommand _summarizeCommand;
  Commands::EnumerateCommand _enumerateCommand;
  Commands::ListCommand _listCommand;
  Commands::ShowCommand _showCommand;
  Commands::DescribeCommand<Offset> _describeCommand;
  Commands::ExplainCommand<Offset> _explainCommand;
  ThreadMapCommands::CountStacks<Offset> _countStacksSubcommand;
  ThreadMapCommands::ListStacks<Offset> _listStacksSubcommand;
  ThreadMapCommands::DescribeStacks<Offset> _describeStacksSubcommand;
  ModuleCommands::ListModules<Offset> _listModulesSubcommand;
  VirtualAddressMapCommands::CountRanges<Offset> _countInaccessibleSubcommand;
  VirtualAddressMapCommands::SummarizeRanges<Offset>
      _summarizeInaccessibleSubcommand;
  VirtualAddressMapCommands::ListRanges<Offset> _listInaccessibleSubcommand;
  VirtualAddressMapCommands::DescribeRanges<Offset>
      _describeInaccessibleSubcommand;
  VirtualAddressMapCommands::CountRanges<Offset> _countReadOnlySubcommand;
  VirtualAddressMapCommands::SummarizeRanges<Offset>
      _summarizeReadOnlySubcommand;
  VirtualAddressMapCommands::ListRanges<Offset> _listReadOnlySubcommand;
  VirtualAddressMapCommands::DescribeRanges<Offset> _describeReadOnlySubcommand;
  VirtualAddressMapCommands::CountRanges<Offset> _countRXOnlySubcommand;
  VirtualAddressMapCommands::SummarizeRanges<Offset> _summarizeRXOnlySubcommand;
  VirtualAddressMapCommands::ListRanges<Offset> _listRXOnlySubcommand;
  VirtualAddressMapCommands::DescribeRanges<Offset> _describeRXOnlySubcommand;
  VirtualAddressMapCommands::CountRanges<Offset> _countWritableSubcommand;
  VirtualAddressMapCommands::SummarizeRanges<Offset>
      _summarizeWritableSubcommand;
  VirtualAddressMapCommands::ListRanges<Offset> _listWritableSubcommand;
  VirtualAddressMapCommands::DescribeRanges<Offset> _describeWritableSubcommand;
  Allocations::Subcommands::SummarizeSignatures<Offset>
      _summarizeSignaturesSubcommand;

  void RegisterSubcommand(Commands::Runner& runner,
                          Commands::Subcommand& subcommand) {
    const std::string& commandName = subcommand.GetCommandName();
    const std::string& setName = subcommand.GetSetName();
    Commands::Command* command = runner.FindCommand(commandName);
    if (command == 0) {
      std::cerr << "Attempted to register subcommand \"" << commandName << " "
                << setName << "\" for command that does\nnot exist.\n";
      return;
    }
    Commands::SetBasedCommand* setBasedCommand =
        dynamic_cast<typename Commands::SetBasedCommand*>(command);
    if (setBasedCommand == 0) {
      std::cerr << "Attempted to register subcommand \"" << commandName << " "
                << setName << " for command that is\nnot set based.\n";
      return;
    }
    setBasedCommand->AddSubcommand(subcommand);
  }

 private:
  Allocations::Subcommands::DefaultSubcommands<Offset>
      _defaultAllocationsSubcommands;
  DequeMapRecognizer<Offset> _dequeMapRecognizer;
  UnorderedMapOrSetBucketsRecognizer<Offset>
      _unorderedMapOrSetBucketsRecognizer;
  UnorderedMapOrSetNodeRecognizer<Offset> _unorderedMapOrSetNodeRecognizer;
  MapOrSetNodeRecognizer<Offset> _mapOrSetNodeRecognizer;
  VectorBodyRecognizer<Offset> _vectorBodyRecognizer;
  DoublyLinkedListNodeRecognizer<Offset> _doublyLinkedListNodeRecognizer;
};

}  // namespace chap
