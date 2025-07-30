import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'settings_screen.dart';
import 'dart:convert';

class ProfileSelectionScreen extends StatefulWidget {
  static const routeName = '/ProfileSelection';
  const ProfileSelectionScreen({Key? key}) : super(key: key);

  @override
  State<ProfileSelectionScreen> createState() => _ProfileSelectionScreenState();
}

class _ProfileSelectionScreenState extends State<ProfileSelectionScreen> {
  final DatabaseReference _databaseRef = FirebaseDatabase.instance.ref();
  String? _selectedProfile;
  String? _activeProfile;
  final List<String> _profiles = ['DEFAULT', 'HOME', 'OUTDOOR'];
  bool _isLoading = false;

  @override
  void initState() {
    super.initState();
    _loadActiveProfile();
  }

  String? get _userId => FirebaseAuth.instance.currentUser?.uid;

  Future<void> _loadActiveProfile() async {
    try {
      if (_userId == null) return;

      final snapshot = await _databaseRef
          .child('Users/$_userId/activeProfile')
          .get();

      if (snapshot.value != null) {
        setState(() {
          _activeProfile = snapshot.value.toString();
          _selectedProfile = _activeProfile;
        });
      } else {
        setState(() {
          _activeProfile = 'DEFAULT';
          _selectedProfile = 'DEFAULT';
        });
      }
    } catch (e) {
      print('Error loading active profile: $e');
      setState(() {
        _activeProfile = 'DEFAULT';
        _selectedProfile = 'DEFAULT';
      });
    }
  }

  Future<void> _selectProfile(String profileName) async {
    if (_userId == null) {
      _showErrorDialog('User not authenticated');
      return;
    }

    setState(() {
      _isLoading = true;
    });

    try {
      // 1. Load profile settings
      final profileSnapshot = await _databaseRef
          .child('Users/$_userId/profiles/$profileName')
          .get();

      Map<String, dynamic> profileSettings = {};

      if (profileSnapshot.value != null) {
        profileSettings = Map<String, dynamic>.from(profileSnapshot.value as Map);
      } else {
        // If profile doesn't exist, create default settings
        profileSettings = _getDefaultSettings();
        await _databaseRef
            .child('Users/$_userId/profiles/$profileName')
            .set(profileSettings);
      }

      // 2. Copy profile settings to main settings
      await _databaseRef
          .child('System_Settings/settings')
          .set(profileSettings);

      // 3. Update active profile
      await _databaseRef
          .child('Users/$_userId/activeProfile')
          .set(profileName);

      setState(() {
        _activeProfile = profileName;
        _selectedProfile = profileName;
        _isLoading = false;
      });

      _showSuccessDialog('Profile "$profileName" activated successfully!');

    } catch (e) {
      setState(() {
        _isLoading = false;
      });
      _showErrorDialog('Error activating profile: $e');
    }
  }

  Map<String, dynamic> _getDefaultSettings() {
    return {
      'mode': 'Both',
      'alertMethod': 'Time To Impact',
      'enableAlert1': 'Enable',
      'enableAlert2': 'Disable',
      'enableAlert3': 'Disable',
      'alertTiming1': 2.0,
      'alertTiming2': 1.5,
      'alertTiming3': 1.0,
      'alertVibration1': 'Double',
      'alertVibration2': 'Pulse',
      'alertVibration3': 'Pulse',
      'alertSound1': 'Collision_warning_hebrew',
      'alertSound2': 'Alarm_clock_4_beeps',
      'alertSound3': 'Alert1',
      'userHeight': 175,
      'systemHeight': 80,
      'volume': 3,
      'minimalHeight': 90,
      'headSafetyMargin': 10,
      'enableCamera': 'Enable'
    };
  }

  void _editProfile(String profileName) {
    Navigator.push(
      context,
      MaterialPageRoute(
        builder: (context) => SettingsQuestionnaire(
          profileName: profileName,
          isEditingProfile: true,
        ),
      ),
    ).then((_) {
      // Refresh the screen when returning from settings
      _loadActiveProfile();
    });
  }

  void _showSuccessDialog(String message) {
    showDialog(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: const Row(
            children: [
              Icon(Icons.check_circle_outline, color: Colors.green),
              SizedBox(width: 8),
              Text('Success'),
            ],
          ),
          content: Text(message),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(context).pop(),
              child: const Text('OK'),
            ),
          ],
        );
      },
    );
  }

  void _showErrorDialog(String message) {
    showDialog(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: const Row(
            children: [
              Icon(Icons.error_outline, color: Colors.red),
              SizedBox(width: 8),
              Text('Error'),
            ],
          ),
          content: Text(message),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(context).pop(),
              child: const Text('OK'),
            ),
          ],
        );
      },
    );
  }

  Widget _buildProfileCard(String profileName) {
    IconData profileIcon;
    Color profileColor;
    String profileDescription;

    switch (profileName) {
      case 'HOME':
        profileIcon = Icons.home;
        profileColor = Colors.green;
        profileDescription = 'Indoor settings for home use';
        break;
      case 'OUTDOOR':
        profileIcon = Icons.nature;
        profileColor = Colors.orange;
        profileDescription = 'Outdoor settings for external use';
        break;
      default:
        profileIcon = Icons.settings;
        profileColor = Colors.blue;
        profileDescription = 'Default system settings';
    }

    bool isActive = _activeProfile == profileName;
    bool isSelected = _selectedProfile == profileName;

    return Card(
      elevation: isActive ? 8 : 2,
      color: isActive ? profileColor.withOpacity(0.1) : null,
      child: InkWell(
        onTap: () {
          setState(() {
            _selectedProfile = profileName;
          });
        },
        child: Container(
          padding: const EdgeInsets.all(16),
          decoration: BoxDecoration(
            border: isSelected ? Border.all(color: profileColor, width: 2) : null,
            borderRadius: BorderRadius.circular(4),
          ),
          child: Column(
            children: [
              Row(
                children: [
                  Icon(
                    profileIcon,
                    size: 32,
                    color: profileColor,
                  ),
                  const SizedBox(width: 12),
                  Expanded(
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Row(
                          children: [
                            Text(
                              profileName,
                              style: TextStyle(
                                fontSize: 18,
                                fontWeight: FontWeight.bold,
                                color: profileColor,
                              ),
                            ),
                            if (isActive) ...[
                              const SizedBox(width: 8),
                              Container(
                                padding: const EdgeInsets.symmetric(
                                  horizontal: 8,
                                  vertical: 2,
                                ),
                                decoration: BoxDecoration(
                                  color: Colors.green,
                                  borderRadius: BorderRadius.circular(12),
                                ),
                                child: const Text(
                                  'ACTIVE',
                                  style: TextStyle(
                                    color: Colors.white,
                                    fontSize: 10,
                                    fontWeight: FontWeight.bold,
                                  ),
                                ),
                              ),
                            ],
                          ],
                        ),
                        const SizedBox(height: 4),
                        Text(
                          profileDescription,
                          style: TextStyle(
                            fontSize: 14,
                            color: Colors.grey[600],
                          ),
                        ),
                      ],
                    ),
                  ),
                ],
              ),
              const SizedBox(height: 12),
              Row(
                mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                children: [
                  Expanded(
                    child: ElevatedButton.icon(
                      onPressed: isActive ? null : () => _selectProfile(profileName),
                      icon: Icon(
                        isActive ? Icons.check : Icons.play_arrow,
                        size: 16,
                      ),
                      label: Text(
                        isActive ? 'Active' : 'Select',
                        style: const TextStyle(fontSize: 12),
                      ),
                      style: ElevatedButton.styleFrom(
                        backgroundColor: isActive ? Colors.grey : profileColor,
                        foregroundColor: Colors.white,
                        padding: const EdgeInsets.symmetric(vertical: 8),
                      ),
                    ),
                  ),
                  const SizedBox(width: 8),
                  Expanded(
                    child: OutlinedButton.icon(
                      onPressed: () => _editProfile(profileName),
                      icon: const Icon(Icons.edit, size: 16),
                      label: const Text(
                        'Edit',
                        style: TextStyle(fontSize: 12),
                      ),
                      style: OutlinedButton.styleFrom(
                        foregroundColor: profileColor,
                        side: BorderSide(color: profileColor),
                        padding: const EdgeInsets.symmetric(vertical: 8),
                      ),
                    ),
                  ),
                ],
              ),
            ],
          ),
        ),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Profile Selection'),
        backgroundColor: Theme.of(context).colorScheme.primaryContainer,
      ),
      body: _isLoading
          ? const Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            CircularProgressIndicator(),
            SizedBox(height: 16),
            Text('Activating profile...'),
          ],
        ),
      )
          : Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(
              'Choose your profile:',
              style: Theme.of(context).textTheme.headlineSmall?.copyWith(
                fontWeight: FontWeight.bold,
              ),
            ),
            const SizedBox(height: 8),
            Text(
              'Each profile has its own settings. Select a profile to activate it, or edit to customize its settings.',
              style: TextStyle(
                color: Colors.grey[600],
                fontSize: 14,
              ),
            ),
            const SizedBox(height: 24),
            Expanded(
              child: ListView.builder(
                itemCount: _profiles.length,
                itemBuilder: (context, index) {
                  return Padding(
                    padding: const EdgeInsets.only(bottom: 16),
                    child: _buildProfileCard(_profiles[index]),
                  );
                },
              ),
            ),
          ],
        ),
      ),
    );
  }
}