<?php

// $Id:$

/**
 * @file
 * Bean Admin Page
 */

/**
 * UI controller.
 */
class BeanTypeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage beans, including fields.';
    return $items;
  }
}

/**
 * Generates the bean type editing form.
 */
function bean_type_form($form, &$form_state, $bean_type, $op = 'edit') {

  if ($op == 'clone') {
    $bean_type->label .= ' (cloned)';
    $bean_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $bean_type->label,
    '#description' => t('The human-readable name of this block type.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($bean_type->type) ? $bean_type->type : '',
    '#maxlength' => 32,
    '#disabled' => $bean_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'bean_get_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this block type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Block type'),
    '#weight' => 40,
  );

  if (!$bean_type->isLocked() && $op != 'add') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete Block type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('bean_type_form_submit')
    );
  }
  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function bean_type_form_submit(&$form, &$form_state) {
  $bean_type = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  $bean_type->save();
  $form_state['redirect'] = 'admin/structure/beans';
}

/**
 * Form API submit callback for the delete button.
 */
function bean_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/beans/manage/' . $form_state['bean_type']->type . '/delete';
}
