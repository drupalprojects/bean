<?php

// $Id:$

/**
 * @file
 * Bean Admin Page
 */

/**
 * Main page callback on the bean type
 */
function bean_type_admin_page() {
  $field_ui = module_exists('field_ui');
  $header = array(t('Name'), array('data' => t('Operations'), 'colspan' => $field_ui ? '4' : '2'));
  $rows = array();

  foreach (bean_get_types() as $type => $bean_type) {
    $row = array();
    $row[] = array('data' => $bean_type->getLabel());

    if ($field_ui) {
      // Manage fields.
      $row[] = array('data' => l(t('manage fields'), 'admin/structure/beans/manage/' . $bean_type->buildURL() . '/fields'));

      // Display fields.
      $row[] = array('data' => l(t('manage display'), 'admin/structure/beans/manage/' . $bean_type->buildURL() . '/display'));
    }
    $rows[] = $row;
  }

  $build['bean_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('There are no Bean Types Available'),
  );
  return $build;
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
