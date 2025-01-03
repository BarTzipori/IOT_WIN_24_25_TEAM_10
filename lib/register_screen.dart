import 'package:flutter/material.dart';

class RegisterScreen extends StatefulWidget {
  static const routeName = "/RegisterScreen";
  const RegisterScreen({Key? key}) : super(key: key);

  @override
  State<RegisterScreen> createState() => _RegisterScreenState();
}

class _RegisterScreenState extends State<RegisterScreen> {
  bool obscureText = true;
  late TextEditingController _nameController;
  late TextEditingController _emailController;
  late TextEditingController _passwordController;
  late TextEditingController _repeatPasswordController;

  final _formKey = GlobalKey<FormState>();
  bool _isLoading = false;

  @override
  void initState() {
    _nameController = TextEditingController();
    _emailController = TextEditingController();
    _passwordController = TextEditingController();
    _repeatPasswordController = TextEditingController();
    super.initState();
  }

  @override
  void dispose() {
    _nameController.dispose();
    _emailController.dispose();
    _passwordController.dispose();
    _repeatPasswordController.dispose();
    super.dispose();
  }

  Future<void> _register() async {
    if (_formKey.currentState!.validate()) {
      setState(() {
        _isLoading = true;
      });

      await Future.delayed(const Duration(seconds: 2)); // Simulate a delay for registration process

      setState(() {
        _isLoading = false;
      });

      _showSuccessDialog("Registration successful!");
    }
  }

  void _showErrorDialog(String message) {
    showDialog(
      context: context,
      builder: (ctx) => AlertDialog(
        title: const Text("Error"),
        content: Text(message),
        actions: [
          TextButton(
            onPressed: () {
              Navigator.of(ctx).pop();
            },
            child: const Text("Okay"),
          ),
        ],
      ),
    );
  }

  void _showSuccessDialog(String message) {
    showDialog(
      context: context,
      builder: (ctx) => AlertDialog(
        title: const Text("Success"),
        content: Text(message),
        actions: [
          TextButton(
            onPressed: () {
              Navigator.of(ctx).pop();
            },
            child: const Text("Okay"),
          ),
        ],
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("Register"),
      ),
      body: SingleChildScrollView(
        padding: const EdgeInsets.all(16.0),
        child: Form(
          key: _formKey,
          child: Column(
            children: [
              TextFormField(
                controller: _nameController,
                decoration: const InputDecoration(labelText: "Name"),
                validator: (value) {
                  if (value == null || value.trim().isEmpty) {
                    return "Please enter your name.";
                  }
                  return null;
                },
              ),
              TextFormField(
                controller: _emailController,
                decoration: const InputDecoration(labelText: "Email"),
                keyboardType: TextInputType.emailAddress,
                validator: (value) {
                  if (value == null || value.trim().isEmpty) {
                    return "Please enter your email.";
                  }
                  if (!RegExp(r"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4}").hasMatch(value)) {
                    return "Please enter a valid email address.";
                  }
                  return null;
                },
              ),
              TextFormField(
                controller: _passwordController,
                decoration: InputDecoration(
                  labelText: "Password",
                  suffixIcon: IconButton(
                    icon: Icon(
                      obscureText ? Icons.visibility_off : Icons.visibility,
                    ),
                    onPressed: () {
                      setState(() {
                        obscureText = !obscureText;
                      });
                    },
                  ),
                ),
                obscureText: obscureText,
                validator: (value) {
                  if (value == null || value.isEmpty) {
                    return "Please enter a password.";
                  }
                  if (value.length < 6) {
                    return "Password must be at least 6 characters long.";
                  }
                  return null;
                },
              ),
              TextFormField(
                controller: _repeatPasswordController,
                decoration: const InputDecoration(labelText: "Repeat Password"),
                obscureText: true,
                validator: (value) {
                  if (value != _passwordController.text) {
                    return "Passwords do not match.";
                  }
                  return null;
                },
              ),
              const SizedBox(height: 20),
              _isLoading
                  ? const CircularProgressIndicator()
                  : ElevatedButton(
                onPressed: _register,
                child: const Text("Register"),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
