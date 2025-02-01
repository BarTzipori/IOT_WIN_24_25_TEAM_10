import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

class SettingsItem {
  SettingsItem({
    required this.expandedValue,
    required this.headerValue,
    required this.options,
    this.isExpanded = false,
    this.selectedOption,
    this.isTextField = false,
    this.textController,
    this.dependsOn,
    this.isNumericRange = false,
    this.minValue,
    this.maxValue,
  });

  List<String> options;
  String headerValue;
  String expandedValue;
  bool isExpanded;
  String? selectedOption;
  bool isTextField;
  TextEditingController? textController;
  String? dependsOn;
  bool isNumericRange;
  double? minValue;
  double? maxValue;
}

class SettingsQuestionnaire extends StatefulWidget {
  const SettingsQuestionnaire({super.key});

  @override
  State<SettingsQuestionnaire> createState() => _SettingsQuestionnaireState();
}

class _SettingsQuestionnaireState extends State<SettingsQuestionnaire> {
  late List<SettingsItem> _data;
  final DatabaseReference _databaseRef = FirebaseDatabase.instance.ref();
  String _esp32Url = "http://172.20.10.10";
  String _connectionStatus = "Not Connected";

  final Map<String, TextEditingController> _controllers = {};

  @override
  void initState() {
    super.initState();
    _data = generateItems();
    for (var item in _data) {
      if (item.isTextField) {
        _controllers[item.headerValue] = TextEditingController();
        item.textController = _controllers[item.headerValue];
      }
    }
    _fetchLocalIP(); // Fetch localIP during initialization
  }

  void _fetchLocalIP() async {
    try {
      DataSnapshot snapshot = await _databaseRef.child('System_Settings/localIP').get();
      if (snapshot.value != null) {
        setState(() {
          _esp32Url = "http://${snapshot.value}";
        });
        _connectionStatus = "Connected to $_esp32Url";
      }
    } catch (e) {
      setState(() {
        _connectionStatus = "Failed to fetch local IP";
      });
      print('Error fetching local IP: $e');
    }
  }


  @override
  void dispose() {
    _controllers.forEach((_, controller) => controller.dispose());
    super.dispose();
  }

  List<String> validateSettings() {
    List<String> errors = [];
    String? alertMethod = _data.firstWhere((item) => item.headerValue == '2. Alert Method').selectedOption;

    if (alertMethod == 'Time to Impact') {
      double? timing1 = double.tryParse(_data.firstWhere((item) => item.headerValue == '6. Alert 1 Timing').selectedOption ?? '');
      double? timing2 = double.tryParse(_data.firstWhere((item) => item.headerValue == '7. Alert 2 Timing').selectedOption ?? '');
      double? timing3 = double.tryParse(_data.firstWhere((item) => item.headerValue == '8. Alert 3 Timing').selectedOption ?? '');

      if (timing1 != null && timing2 != null) {
        // Check if they follow the required order
        if (!( timing2 < timing1 )) {
          errors.add('Alert timings must follow the order: Alert 2 < Alert 1');
        }
      }

      // Check if all timings are set
      if (timing1 != null && timing2 != null && timing3 != null) {
        // Check if they follow the required order
        if (!(timing3 < timing2 || timing2 < timing1 || timing3 < timing1)) {
          errors.add('Alert timings must follow the order: Alert 3 < Alert 2 < Alert 1');
        }
      }
    } else if (alertMethod == 'Distance') {
      double? distance1 = double.tryParse(_controllers['9. Alert 1 Distance']?.text ?? '');
      double? distance2 = double.tryParse(_controllers['10. Alert 2 Distance']?.text ?? '');
      double? distance3 = double.tryParse(_controllers['11. Alert 3 Distance']?.text ?? '');

      // Check range validation
      if (distance1 != null && (distance1 < 20 || distance1 > 250)) {
        errors.add('Alert 1 distance must be between [20cm - 250cm]');
      }

      if (distance2 != null && (distance2 < 20 || distance2 > 250)) {
        errors.add('Alert 2 distance must be between [20cm - 250cm]');
      }

      if (distance3 != null && (distance3 < 20 || distance3 > 250)) {
        errors.add('Alert 3 distance must be between [20cm - 250cm]');
      }

      if (distance1 != null && distance2 != null) {
        // Check if they follow the required order
        if (!( distance2 < distance1 )) {
          errors.add('Alert distances must follow the order: Alert 2 < Alert 1');
        }
      }

      // Check if all distances are set and within range
      if (distance1 != null && distance2 != null && distance3 != null) {
        // Check if they follow the required order
        if (!(distance3 < distance2 || distance2 < distance1 || distance3 < distance1 )) {
          errors.add('Alert distances must follow the order: Alert 3 < Alert 2 < Alert 1');
        }
      }
    }
    return errors;
  }

  List<double> _generateTimingOptions() {
    List<double> options = [];
    for (double i = 0.5; i <= 2.5; i += 0.1) {
      options.add(double.parse(i.toStringAsFixed(1)));
    }
    return options;
  }

  List<SettingsItem> generateItems() {
    List<double> timingOptions = _generateTimingOptions();

    return [
      SettingsItem(
        headerValue: '1. Mode',
        expandedValue: 'Choose the alert mode',
        options: ['Sound', 'Vibration', 'Both'],
      ),
      SettingsItem(
        headerValue: '2. Alert Method',
        expandedValue: 'Choose between time to impact or distance',
        options: ['Time to Impact', 'Distance'],
      ),
      SettingsItem(
        headerValue: '3. Alert 1',
        expandedValue: 'Enable or disable Alert 1',
        options: ['Enable', 'Disable'],
      ),
      SettingsItem(
        headerValue: '4. Alert 2',
        expandedValue: 'Enable or disable Alert 2',
        options: ['Enable', 'Disable'],
      ),
      SettingsItem(
        headerValue: '5. Alert 3',
        expandedValue: 'Enable or disable Alert 3',
        options: ['Enable', 'Disable'],
      ),
      SettingsItem(
        headerValue: '6. Alert 1 Timing',
        expandedValue: 'Set timing for Alert 1',
        options: timingOptions.map((e) => e.toString()).toList(),
      ),
      SettingsItem(
        headerValue: '7. Alert 2 Timing',
        expandedValue: 'Set timing for Alert 2',
        options: timingOptions.map((e) => e.toString()).toList(),
        dependsOn: '4',
      ),
      SettingsItem(
        headerValue: '8. Alert 3 Timing',
        expandedValue: 'Set timing for Alert 3',
        options: timingOptions.map((e) => e.toString()).toList(),
        dependsOn: '5',
      ),
      SettingsItem(
        headerValue: '9. Alert 1 Distance',
        expandedValue: 'Enter distance for Alert 1 [20cm - 250cm]',
        options: [],
        isTextField: true,
        isNumericRange: true,
        minValue: 20,
        maxValue: 250,
      ),
      SettingsItem(
        headerValue: '10. Alert 2 Distance',
        expandedValue: 'Enter distance for Alert 2 (20cm - 250cm)',
        options: [],
        isTextField: true,
        isNumericRange: true,
        minValue: 20,
        maxValue: 250,
        dependsOn: '4',
      ),
      SettingsItem(
        headerValue: '11. Alert 3 Distance',
        expandedValue: 'Enter distance for Alert 2 (20cm - 250cm)',
        options: [],
        isTextField: true,
        isNumericRange: true,
        minValue: 20,
        maxValue: 250,
        dependsOn: '5',
      ),
      SettingsItem(
        headerValue: '12. Alert 1 Vibration',
        expandedValue: 'Choose vibration type for Alert 1',
        options: ['Short', 'Long', 'Double', 'Pulse'],
      ),
      SettingsItem(
        headerValue: '13. Alert 2 Vibration',
        expandedValue: 'Choose vibration type for Alert 2',
        options: ['Short', 'Long', 'Double', 'Pulse'],
        dependsOn: '4',
      ),
      SettingsItem(
        headerValue: '14. Alert 3 Vibration',
        expandedValue: 'Choose vibration type for Alert 3',
        options: ['Short', 'Long', 'Double', 'Pulse'],
        dependsOn: '5',
      ),
      SettingsItem(
        headerValue: '15. Alert 1 Sound',
        expandedValue: 'Choose sound type for Alert 1',
        options: ['Collision_warning_hebrew',
          'Alarm_clock_4_beeps',
          'Alert1',
          'Alert2',
          'Beep_2',
          'Warning_beeps',
          'Notification_sound',
          'Beep_3'],
      ),
      SettingsItem(
        headerValue: '16. Alert 2 Sound',
        expandedValue: 'Choose sound type for Alert 2',
        options: ['Collision_warning_hebrew',
          'Alarm_clock_4_beeps',
          'Alert1',
          'Alert2',
          'Beep_2',
          'Warning_beeps',
          'Notification_sound',
          'Beep_3'],
        dependsOn: '4',
      ),
      SettingsItem(
        headerValue: '17. Alert 3 Sound',
        expandedValue: 'Choose sound type for Alert 3',
        options: ['Collision_warning_hebrew',
          'Alarm_clock_4_beeps',
          'Alert1',
          'Alert2',
          'Beep_2',
          'Warning_beeps',
          'Notification_sound',
          'Beep_3'],
        dependsOn: '5',
      ),
      SettingsItem(
        headerValue: '18. User Height',
        expandedValue: 'Enter user height in centimeters',
        options: [],
        isTextField: true,
      ),
      SettingsItem(
        headerValue: '19. System Height',
        expandedValue: 'Enter system height in centimeters',
        options: [],
        isTextField: true,
      ),
      SettingsItem(
        headerValue: '20. Volume Sound',
        expandedValue: 'Set sound volume (1-5)',
        options: List.generate(5, (index) => (index + 1).toString()),
      ),
      SettingsItem(
        headerValue: '21. Minimal Height',
        expandedValue: 'Enter minimal height in centimeters',
        options: [],
        isTextField: true,
        isNumericRange: true,
        minValue: 50,
        maxValue: 200,
      ),
      SettingsItem(
        headerValue: '22. Head Safety Margin',
        expandedValue: 'Enter head safety margin in centimeters',
        options: [],
        isTextField: true,
        isNumericRange: true,
        minValue: 5,
        maxValue: 20,
      ),
    ];
  }

  Future<void> _saveSettingsToDatabase() async {
    // Collect all validation errors
    List<String> errors = validateSettings();

    // Add validation for new height fields
    double? minimalHeight = double.tryParse(_controllers['21. Minimal Height']?.text ?? '');
    double? headSafetyMargin = double.tryParse(_controllers['22. Head Safety Margin']?.text ?? '');
    double? userHeight = double.tryParse(_controllers['18. User Height']?.text ?? '');
    double? systemHeight = double.tryParse(_controllers['19. System Height']?.text ?? '');

    // Validate minimal height
    if (minimalHeight != null) {
      if (minimalHeight < 50 || minimalHeight > 200) {  // Assuming reasonable range for minimal height
        errors.add('Minimal height must be between 50cm and 200cm');
      }
      if (userHeight != null) {
        if (minimalHeight > userHeight) {
          errors.add('Minimal height cannot be greater than user height');
        }
        if (userHeight - minimalHeight < 20) {
          errors.add('Difference between user height and minimal height must be at least 20cm');
        }
      }
    }

    // Validate head safety margin
    if (headSafetyMargin != null) {
      if (headSafetyMargin < 5 || headSafetyMargin > 20) {  // Assuming reasonable range for safety margin
        errors.add('Head safety margin must be between 5cm and 20cm');
      }
      if (userHeight != null && systemHeight != null) {
        double effectiveHeight = userHeight - systemHeight - headSafetyMargin;
        if (effectiveHeight < 0) {
          errors.add('Head safety margin is too large for current user and system heights');
        }
      }
    }



    // If there are any errors, display them and don't save
    if (errors.isNotEmpty) {
      String errorMessage = 'Cannot save due to the following errors:\n' +
          errors.map((e) => '• $e').join('\n');

      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text(errorMessage),
          backgroundColor: Colors.red,
          duration: const Duration(seconds: 5),
        ),
      );
      return;
    }

    try {
      Map<String, dynamic> settingsData = {};

      final Map<String, String> headerToVarName = {
        '1. Mode': 'mode',
        '2. Alert Method': 'alertMethod',
        '3. Alert 1': 'enableAlert1',
        '4. Alert 2': 'enableAlert2',
        '5. Alert 3': 'enableAlert3',
        '6. Alert 1 Timing': 'alertTiming1',
        '7. Alert 2 Timing': 'alertTiming2',
        '8. Alert 3 Timing': 'alertTiming3',
        '9. Alert 1 Distance': 'alertDistance1',
        '10. Alert 2 Distance': 'alertDistance2',
        '11. Alert 3 Distance': 'alertDistance3',
        '12. Alert 1 Vibration': 'alertVibration1',
        '13. Alert 2 Vibration': 'alertVibration2',
        '14. Alert 3 Vibration': 'alertVibration3',
        '15. Alert 1 Sound': 'alertSound1',
        '16. Alert 2 Sound': 'alertSound2',
        '17. Alert 3 Sound': 'alertSound3',
        '18. User Height': 'userHeight',
        '19. System Height': 'systemHeight',
        '20. Volume Sound': 'volume',
        '21. Minimal Height': 'minimalHeight',
        '22. Head Safety Margin': 'headSafetyMargin'
      };

      // Define which fields should be treated as numbers
      final Set<String> numericFields = {
        'alertTiming1',
        'alertTiming2',
        'alertTiming3',
        'alertDistance1',
        'alertDistance2',
        'alertDistance3',
        'userHeight',
        'systemHeight',
        'volume',
        'minimalHeight',
        'headSafetyMargin'
      };

      // Process each setting item
      for (var item in _data) {
        String? dbVarName = headerToVarName[item.headerValue];
        if (dbVarName != null) {
          if (item.isTextField) {
            String value = item.textController?.text.trim() ?? '';
            if (numericFields.contains(dbVarName) && value.isNotEmpty) {
              // Convert to double first for decimal numbers
              double? numValue = double.tryParse(value);
              if (numValue != null) {
                if (dbVarName == 'volume' ||
                    dbVarName == 'userHeight' ||
                    dbVarName == 'systemHeight' ||
                    dbVarName == 'minimalHeight' ||
                    dbVarName == 'headSafetyMargin') {
                  // Convert to integer for specific fields
                  settingsData[dbVarName] = numValue.round();
                } else {
                  // Keep as double for distances and timings
                  settingsData[dbVarName] = numValue;
                }
              }
            } else {
              settingsData[dbVarName] = value;
            }
          } else {
            String value = item.selectedOption ?? '';
            if (numericFields.contains(dbVarName) && value.isNotEmpty) {
              // Convert to double first for decimal numbers
              double? numValue = double.tryParse(value);
              if (numValue != null) {
                if (dbVarName == 'volume') {
                  // Convert to integer for volume
                  settingsData[dbVarName] = numValue.round();
                } else {
                  // Keep as double for distances and timings
                  settingsData[dbVarName] = numValue;
                }
              }
            } else {
              settingsData[dbVarName] = value;
            }
          }
        }
      }

      // Print settings data before saving (for debugging)
      print('Settings Data to Save: $settingsData');

      // Remove any empty values
      settingsData.removeWhere((key, value) => value.toString().isEmpty);

      // Save to Firebase
      await _databaseRef.child('System_Settings/settings').update(settingsData);

      // Show success message
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(
          content: Text('Settings saved successfully!'),
          backgroundColor: Colors.green,
        ),
      );

      // Optionally log the saved data
      final DataSnapshot snapshot = await _databaseRef.child('System_Settings/settings').get();
      print('Saved settings in database: ${snapshot.value}');

    } catch (e) {
      print('Error saving settings to Firebase: $e');
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text('Error saving settings: $e'),
          backgroundColor: Colors.red,
        ),
      );
    }
  }

  Future<void> _testConnection() async {
    try {
      final response = await http.get(Uri.parse(_esp32Url));
      setState(() {
        _connectionStatus = response.statusCode == 200
            ? "Connected to ESP32"
            : "Connection failed: ${response.statusCode}";
      });
    } catch (e) {
      setState(() {
        _connectionStatus = "Connection error: $e";
      });
    }
  }

  Future<void> _makeSound(String sound) async {
    try {
      final url = Uri.parse("$_esp32Url/play_sound=${Uri.encodeComponent(sound)}");
      final response = await http.get(url);

      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text(response.statusCode == 200
              ? 'Playing $sound'
              : 'Failed to play $sound. Status: ${response.statusCode}'),
        ),
      );
    } catch (e) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Error: $e')),
      );
    }
  }

  Future<void> _makeVibration(String vibration) async {
    try {
      final url = Uri.parse("$_esp32Url/play_vibration=${Uri.encodeComponent(vibration)}");
      final response = await http.get(url);

      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
          content: Text(response.statusCode == 200
              ? 'Playing $vibration'
              : 'Failed to play $vibration. Status: ${response.statusCode}'),
        ),
      );
    } catch (e) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Error: $e')),
      );
    }
  }

  bool _shouldShowItem(SettingsItem item) {
    // First check if the item should be hidden based on mode selection
    String? selectedMode = _data.firstWhere(
          (element) => element.headerValue == '1. Mode',
      orElse: () => _data[0],
    ).selectedOption;

    // Get selected alert method
    String? alertMethod = _data.firstWhere(
          (element) => element.headerValue == '2. Alert Method',
      orElse: () => _data[0],
    ).selectedOption;

    // Hide timing panels when Distance is selected
    if (alertMethod == 'Distance') {
      if (item.headerValue == '6. Alert 1 Timing' ||
          item.headerValue == '7. Alert 2 Timing' ||
          item.headerValue == '8. Alert 3 Timing') {
        return false;
      }
    }

    // Hide distance panels when Time to Impact is selected
    if (alertMethod == 'Time to Impact') {
      if (item.headerValue == '9. Alert 1 Distance' ||
          item.headerValue == '10. Alert 2 Distance' ||
          item.headerValue == '11. Alert 3 Distance') {
        return false;
      }
    }

    // Handle vibration panels visibility
    if (item.headerValue.contains('Vibration')) {
      if (selectedMode == 'Sound') {
        return false;
      }
    }

    // Handle sound panels visibility
    if (item.headerValue.contains('Sound') &&
        !item.headerValue.contains('Volume')) {  // Exclude Volume Sound from this check
      if (selectedMode == 'Vibration') {
        return false;
      }
    }

    // Only show Volume Sound setting if mode includes Sound
    if (item.headerValue == '20. Volume Sound') {
      if (selectedMode == 'Vibration') {
        return false;
      }
    }

    // Then check dependencies as before
    if (item.dependsOn == null) return true;

    var dependsOnNumber = item.dependsOn?.split('.')[0].trim();
    final dependentItem = _data.firstWhere(
          (element) => element.headerValue.startsWith(dependsOnNumber ?? ''),
      orElse: () => _data[0],
    );

    return dependentItem.selectedOption == 'Enable';
  }

  @override
  Widget build(BuildContext context) {
    // Filter visible items first
    final visibleItems = _data.where((item) => _shouldShowItem(item)).toList();

    return Scaffold(
      appBar: AppBar(
        title: const Text('System Settings'),
        backgroundColor: Theme.of(context).colorScheme.primaryContainer,
      ),
      body: SingleChildScrollView(
        child: Container(
          padding: const EdgeInsets.all(16.0),
          child: Column(
            children: [
              ExpansionPanelList(
                expandedHeaderPadding: const EdgeInsets.all(0),
                expansionCallback: (int index, bool isExpanded) {
                  final originalIndex = _data.indexOf(visibleItems[index]);
                  setState(() {
                    _data[originalIndex].isExpanded = !_data[originalIndex].isExpanded;
                  });
                },
                children: visibleItems.map<ExpansionPanel>((SettingsItem item) {
                  return ExpansionPanel(
                    headerBuilder: (BuildContext context, bool isExpanded) {
                      return ListTile(
                        title: Text(
                          item.headerValue,
                          style: const TextStyle(fontWeight: FontWeight.bold),
                        ),
                        subtitle: Text(item.expandedValue),
                      );
                    },
                    body: item.isTextField
                        ? Padding(
                      padding: const EdgeInsets.all(16.0),
                      child: TextField(
                        controller: item.textController,
                        decoration: InputDecoration(
                          labelText: item.isNumericRange
                              ? 'Enter value (${item.minValue}-${item.maxValue}cm)'
                              : 'Enter value',
                          border: const OutlineInputBorder(),
                          suffixText: item.headerValue.toLowerCase().contains('height')
                              ? 'cm'
                              : (item.isNumericRange ? 'cm' : null),
                        ),
                        keyboardType: const TextInputType.numberWithOptions(decimal: true),
                        onChanged: (value) {},
                      ),
                    )
                        : Column(
                      children: item.options.map((String option) {
                        return Row(
                          mainAxisAlignment: MainAxisAlignment.spaceBetween,
                          children: [
                            Expanded(
                              child: RadioListTile<String>(
                                title: Text(option),
                                value: option,
                                groupValue: item.selectedOption,
                                onChanged: (String? value) {
                                  setState(() {
                                    item.selectedOption = value;
                                  });
                                },
                              ),
                            ),
                            if (item.headerValue.contains('Sound') ||
                                item.headerValue.contains('Vibration'))
                              Padding(
                                padding: const EdgeInsets.only(right: 16.0),
                                child: ElevatedButton(
                                  onPressed: () {
                                    if (item.headerValue.contains('Sound')) {
                                      _makeSound(option);
                                    } else {
                                      _makeVibration(option);
                                    }
                                  },
                                  child: const Text('Play'),
                                ),
                              ),
                          ],
                        );
                      }).toList(),
                    ),
                    isExpanded: item.isExpanded,
                  );
                }).toList(),
              ),
              const SizedBox(height: 16),
              ElevatedButton(
                onPressed: _testConnection,
                child: const Text('Test Connection'),
              ),
              Text('Connection Status: $_connectionStatus'),
              const SizedBox(height: 16),
              ElevatedButton(
                onPressed: _saveSettingsToDatabase,
                child: const Text('Save Settings'),
              ),
              const SizedBox(height: 32),
            ],
          ),
        ),
      ),
    );
  }
}